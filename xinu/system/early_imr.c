/*
 * early_imr.c
 *
 * Copyright (c) 2012, 2013 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <xinu.h>

#define INTEL_CLN_SB_CMD_ADDR	(0x000000D0)
#define INTEL_CLN_SB_DATA_ADDR	(0x000000D4)

#define INTEL_CLN_SB_MCR_SHIFT	(24)
#define INTEL_CLN_SB_PORT_SHIFT	(16)
#define INTEL_CLN_SB_REG_SHIFT	(8)
#define INTEL_CLN_SB_BYTEEN	(0xF0)	/* Enable all 32 bits */

/* IMR related reg address */
#define CFG_READ_OPCODE		(0x10)  /* Control Read */
#define CFG_WRITE_OPCODE	(0x11)  /* Control write */
#define DRAM_IMR0L		(0x40)  /* IMR0RL address */
#define DRAM_IMR0H		(0x41)  /* IMR0RL address */
#define DRAM_IMR0RM		(0x42)  /* IMR0RM address */
#define DRAM_IMR0WM		(0x43)  /* IMR0WM address */
#define DRAM_IMR1L		(0x44)  /* IMR1L address */
#define DRAM_IMR1H		(0x45)  /* IMR1H address */
#define DRAM_IMR1RM		(0x46)  /* IMR1RM address */
#define DRAM_IMR1WM		(0x47)  /* IMR1WM address */
#define DRAM_IMR3L		(0x4C)  /* IMR3L address */
#define DRAM_IMR3H		(0x4D)  /* IMR3H address */
#define DRAM_IMR3RM		(0x4E)  /* IMR3RM address */
#define DRAM_IMR3WM		(0x4F)  /* IMR3WM address */
#define DRAM_IMR7L		(0x5C)  /* IMR7RM address */
#define DRAM_IMR7H		(0x5D)  /* IMR7RM address */
#define DRAM_IMR7RM		(0x5E)  /* IMR7RM address */
#define DRAM_IMR7WM		(0x5F)  /* IMR7WM address */

#define IMR_WRITE_ENABLE_ALL	(0xFFFFFFFF)
#define IMR_READ_ENABLE_ALL	(0xBFFFFFFF)
#define IMR_BASE_ADDR		(0x0000)
#define IMR_LOCK_BIT		(0x80000000)

/* Mask of the last 2 bit of IMR address [23:2] */
#define IMR_MASK		(0xFFFFFC)
/* Mask that enables IMR access for Non-SMM Core, Core Snoops Only.*/
#define IMR_SNOOP_NON_SMM_ENABLE (0x40000001)
/* Mask that enables IMR access for Non-SMM Core Only.*/
#define IMR_NON_SMM_ENABLE	(0x00000001)

typedef enum {
	SB_ID_HUNIT = 0x03,
	SB_ID_THERMAL = 0x04,
	SB_ID_ESRAM = 0x05,
}cln_sb_id;

static uint32 sb_pcidev;

/**
 * intel_cln_early_sb_read_reg
 *
 * @param cln_sb_id: Sideband identifier
 * @param command: Command to send to destination identifier
 * @param reg: Target register w/r to cln_sb_id
 * @return nothing
 *
 * Utility function to allow thread-safe read of side-band
 * command - can be different read op-code types - which is why we don't
 * hard-code this value directly into msg
 */
void intel_cln_early_sb_read_reg(cln_sb_id id, unsigned char cmd,
				 unsigned char reg, uint32 *data)
{
	uint32 msg = (cmd << INTEL_CLN_SB_MCR_SHIFT) |
		  ((id << INTEL_CLN_SB_PORT_SHIFT) & 0xFF0000)|
		  ((reg << INTEL_CLN_SB_REG_SHIFT) & 0xFF00)|
		  INTEL_CLN_SB_BYTEEN;

	if (data == NULL) {
		return;
	}

	pci_write_config_dword(sb_pcidev, INTEL_CLN_SB_CMD_ADDR, msg);
	pci_read_config_dword(sb_pcidev, INTEL_CLN_SB_DATA_ADDR, data);

}

/**
 * intel_cln_early_sb_write_reg
 *
 * @param cln_sb_id: Sideband identifier
 * @param command: Command to send to destination identifier
 * @param reg: Target register w/r to cln_sb_id
 * @return nothing
 *
 * Utility function to allow thread-safe write of side-band
 */
void intel_cln_early_sb_write_reg(cln_sb_id id, unsigned char cmd,
				  unsigned char reg, uint32 data)
{
	uint32 msg = (cmd << INTEL_CLN_SB_MCR_SHIFT) |
		  ((id << INTEL_CLN_SB_PORT_SHIFT) & 0xFF0000)|
		  ((reg << INTEL_CLN_SB_REG_SHIFT) & 0xFF00)|
		  INTEL_CLN_SB_BYTEEN;

	pci_write_config_dword(sb_pcidev, INTEL_CLN_SB_DATA_ADDR, data);
	pci_write_config_dword(sb_pcidev, INTEL_CLN_SB_CMD_ADDR, msg);
}

/* Clanton hardware */
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_DEVICE_ID_CLANTON_SB	0x0958

/**
 * sb_probe
 *
 * @param dev: the PCI device matching
 * @param id: entry in the match table
 * @return 0
 *
 * Callback from PCI layer when dev/vendor ids match.
 * Sets up necessary resources
 */
static int intel_cln_early_sb_probe(void)
{
	int	sb_dev;

	sb_dev = find_pci_device(PCI_DEVICE_ID_CLANTON_SB,
				 PCI_VENDOR_ID_INTEL, 0);
	if (sb_dev < 0) {
		kprintf("%s(): error finding PCI device DID 0x%x\n",
			__FUNCTION__, PCI_DEVICE_ID_CLANTON_SB);
		// Reflect the error.
		return sb_dev;
	}
	return 0;
}

/**
 * cln_remove_imr
 *
 * write default values to reg to set imr free
 *
 */
static void cln_remove_imr(unsigned char reg_l, unsigned char reg_h,
			   unsigned char reg_rm, unsigned char reg_wm)
{
	intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
				     reg_rm, IMR_READ_ENABLE_ALL);
	intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
				     reg_wm, IMR_WRITE_ENABLE_ALL);
	intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
				     reg_h, IMR_BASE_ADDR);
	intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
				     reg_l, IMR_BASE_ADDR);
}

/**
 * cln_remove_imr_grub
 *
 * remove imr protection from grub,
 * set imr rw masks to default
 */
static void cln_remove_imr_grub(void)
{
	cln_remove_imr(DRAM_IMR0L, DRAM_IMR0H, DRAM_IMR0RM, DRAM_IMR0WM);
}

/**
 * cln_remove_imr_boot_params
 *
 * remove imr protection from grub,
 * set imr rw masks to default
 */
static void cln_remove_imr_boot_params(void)
{
	cln_remove_imr(DRAM_IMR1L, DRAM_IMR1H, DRAM_IMR1RM, DRAM_IMR1WM);
}

/**
 * cln_remove_imr_bzimage
 *
 * remove imr protection from bzImage,
 * set imr rw masks to default
 */
static void cln_remove_imr_bzimage(void)
{
	cln_remove_imr(DRAM_IMR7L, DRAM_IMR7H, DRAM_IMR7RM, DRAM_IMR7WM);
}

int remove_irm_protections(void)
{
	uint32 tmp_addr;

	if (intel_cln_early_sb_probe() != 0) {
		kprintf("%s() error probing for IRM device\n", __FUNCTION__);
		/* Critial error - bail out */
		return SYSERR;
	}

	intel_cln_early_sb_read_reg(SB_ID_ESRAM, CFG_READ_OPCODE,
				    DRAM_IMR3L, &tmp_addr);
	if (tmp_addr & IMR_LOCK_BIT) {
		return SYSERR;
	}

	if (tmp_addr) {
		intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
					     DRAM_IMR3RM, IMR_READ_ENABLE_ALL);
		intel_cln_early_sb_write_reg(SB_ID_ESRAM, CFG_WRITE_OPCODE,
					     DRAM_IMR3WM,
					     IMR_WRITE_ENABLE_ALL);
	}

	cln_remove_imr_boot_params();
	cln_remove_imr_bzimage();
	cln_remove_imr_grub();

	return OK;
}
