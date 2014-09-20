/* pci.c - pci_init */

#include <xinu.h>

/*
 * Base and length of memory-mapped PCI configuration space.
 *
 * These values are correct for Galileo and many other X86 platforms,
 * but may need to be changed for some platforms.  In the general case, BIOS
 * sets up MMConfig space and passes the base and length values to the OS via
 * ACPI tables.  For simplicity, Xinu avoids parsing ACPI.  Note: If these
 * values are in doubt, one easy way to determine the correct values is to
 * boot Linux and scan its dmesg log file, as follows:
 * 	dmesg -s 30000000 | grep -i mmconfig
 */

#define MMCONFIG_BASE	0xe0000000	/* Base of MMConfig space */
#define MMCONFIG_LEN	0x10000000	/* Length of MMConfig space */

/*------------------------------------------------------------------------
 * pci_init  -  Initialize the PCI bios structures
 *------------------------------------------------------------------------
 */
int32	pci_init(void)
{
	return OK;
}

/*------------------------------------------------------------------------
 * regAddress_MMConfig  -  Get the address of a register in MMConfig space
 *------------------------------------------------------------------------
 */
static inline void *
regAddress_MMConfig(
	unsigned int	mmconfigBase,
	unsigned int	bus,
	unsigned int	device,
	unsigned int	function,
	unsigned int	offset
)
{
	void	*retval;

	retval = (void *)
		(mmconfigBase +
		(bus * PCIE_BYTES_PER_BUS) +
		(device * PCIE_BYTES_PER_DEVICE) +
		(function * PCIE_BYTES_PER_FUNCTION) +
		offset);
	return retval;
}

/*------------------------------------------------------------------------
 * encodePCIDevice - Encode PCI bus/device/func into an an unsigned int
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodePCIDevice(unsigned int bus, unsigned int dev, unsigned int func)
{
	unsigned int	retval;

	retval = (bus << PCI_BUS_SHIFT) | (dev << PCI_DEV_SHIFT) | func;
	return retval;
}

/*------------------------------------------------------------------------
 * encodedPCIDevToBus  -  Extact bus from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodedPCIDevToBus(unsigned int encodedDev)
{
	return ((encodedDev >> PCI_BUS_SHIFT) & PCI_BUS_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToDevice  -  Extract device from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodedPCIDevToDevice(unsigned int encodedDev)
{
	return ((encodedDev >> PCI_DEV_SHIFT) & PCI_DEV_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToFunction - Extract func from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodedPCIDevToFunction(unsigned int encodedDev)
{
	return encodedDev & PCI_FUNC_MASK;
}

/*------------------------------------------------------------------------
 * find_pci_device - find a specified PCI device on the bus
 *------------------------------------------------------------------------
 */
int find_pci_device(int32 deviceID, int32 vendorID, int32 index)
{
	int		count;
	int		multifunction;
	unsigned int	info;
	unsigned int	bus, dev, func;
	struct pci_config_header *devfuncHdr;

	/* Traverse memory-mapped PCI configuration space looking for
	   a match for the target device */
	count = 0;
	for (bus = 0; bus < PCI_MAX_BUSES; bus++) {
		for (dev = 0; dev < PCI_DEVICES_PER_BUS; dev++) {
			for (func = 0; func < PCI_FUNCTIONS_PER_DEVICE;
			     func++) {
				devfuncHdr = regAddress_MMConfig(MMCONFIG_BASE,
						bus, dev, func, 0);
				if ((devfuncHdr->vendorID == vendorID) &&
				    (devfuncHdr->deviceID == deviceID) &&
				    (index == count++)) {
					/* Match: return the matching
					   PCI Bus:Device:Function
					   encoded into an unsigned int. */
					info = encodePCIDevice(bus, dev, func);
					return info;
				}
				/* Evaluate sub-functions only if the device
				   is multi-function */
				if (func == 0) {
					multifunction =
						devfuncHdr->headerType &
						PCI_HDR_TYPE_MULTIFUNCTION;
					if (!multifunction) {
						break; /* Inner for loop. */
					}
				}
			}
		}
	}
	/* The target device was not found. */
	return SYSERR;
}


int pci_read_config_byte(uint32 encodedDev, int offset, unsigned char *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *((uint8 volatile *) reg);
	return OK;
}

int pci_read_config_word(uint32 encodedDev, int offset, unsigned short *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint16 volatile *) reg;
	return OK;
}

int pci_read_config_dword(uint32 encodedDev, int offset, uint32 *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint32 volatile *) reg;
	return OK;
}

int pci_write_config_byte(uint32 encodedDev, int offset, unsigned char value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint8 volatile *) reg = value;
	return OK;
}

int pci_write_config_word(uint32 encodedDev, int offset, unsigned short value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint16 volatile *) reg = value;
	return OK;
}

int pci_write_config_dword(uint32 encodedDev, int offset, uint32 value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint32 volatile *) reg = value;
	return OK;
}

int pci_get_dev_mmio_base_addr(uint32 encodedDev, int barIndex,
			      void **mmio_base_address)
{
	unsigned int	bar_value;
	int		status;

	/* Determine the value of the target PCI device's MMIO base address
	   register */
	bar_value = 0;
	status = pci_read_config_dword(encodedDev,
					offsetof(struct pci_config_header,
						 bars[barIndex]),
					&bar_value);
	if (status < 0) {
		return status;
	}
	if (bar_value & PCI_BAR_REGION_TYPE_IO_MASK) {
		/* Reject port I/O regions */
		return SYSERR;
	}
	if (bar_value & PCI_BAR_LOCATABLE_64B_MASK) {
		/* Reject addresses beyond the 32 bit address range */
		return SYSERR;
	}
	*mmio_base_address = (void *) (bar_value & PCI_BAR_BASE_ADDRESS_MASK);
	return OK;
}
