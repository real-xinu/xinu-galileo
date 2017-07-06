/* pci.c - pci_init */

#include <xinu.h>

/*
 * Base and length of memory-mapped PCI configuration space.
 *
 * These values are correct for Galileo and many other X86 platforms,
 * but may need to be changed for some platforms.  In the general case,
 * BIOS sets up MMConfig space and passes the base and length values to
 * the OS via ACPI tables.  For simplicity, Xinu avoids parsing ACPI.
 * Note: If these values are in doubt, one easy way to determine the
 * correct values is to boot Linux and scan its dmesg log file, as
 * follows:
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
static inline void *regAddress_MMConfig(
		uint32	mmconfigBase,	/* MM Config base address	*/
		uint32	bus,		/* PCI bus number		*/
		uint32	device,		/* PCI device number		*/
		uint32	function,	/* PCI function number		*/
		uint32	offset		/* Offset in config space	*/
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
 * encodePCIDevice - Encode PCI bus/device/func into an an uint32
 *------------------------------------------------------------------------
 */
static inline uint32 encodePCIDevice(
		uint32	bus,	/* PCI bus number	*/
		uint32	dev,	/* PCI device number	*/
		uint32	func	/* PCI function number	*/
		)
{
	uint32	retval;

	retval = (bus << PCI_BUS_SHIFT) | (dev << PCI_DEV_SHIFT) | func;
	return retval;
}

/*------------------------------------------------------------------------
 * encodedPCIDevToBus  -  Extact bus from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline uint32 encodedPCIDevToBus(
		uint32	encodedDev	/* Encoded PCI device	*/
		)
{
	return ((encodedDev >> PCI_BUS_SHIFT) & PCI_BUS_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToDevice  -  Extract device from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline uint32 encodedPCIDevToDevice(
		uint32	encodedDev
		)
{
	return ((encodedDev >> PCI_DEV_SHIFT) & PCI_DEV_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToFunction - Extract func from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline uint32 encodedPCIDevToFunction(
		uint32	encodedDev	/* Encoded PCI device	*/
		)
{
	return encodedDev & PCI_FUNC_MASK;
}

/*------------------------------------------------------------------------
 * find_pci_device - find a specified PCI device on the bus
 *------------------------------------------------------------------------
 */
int32 find_pci_device(
		int32	deviceID,	/* PCI device ID	*/
		int32	vendorID,	/* PCI vendor ID	*/
		int32	index		/* Index of device	*/
		)
{
	int32	count;		/* Count of the device		*/
	int32	multifunction;	/* Is device multifinction?	*/
	uint32	info;		/* Encoded PCI device		*/
	uint32	bus, dev, func;	/* Bus, device, function nos.	*/
	struct pci_config_header *devfuncHdr;
				/* Header pointer for PCI device*/

	/* Traverse memory-mapped PCI configuration space looking for
	   a match for the target device */
	count = 0;
	for (bus = 0; bus < PCI_MAX_BUSES; bus++) {
		for (dev = 0; dev < PCI_DEVICES_PER_BUS; dev++) {
			for (func = 0; func < PCI_FUNCTIONS_PER_DEVICE;
			     func++) {
				devfuncHdr =
					regAddress_MMConfig(MMCONFIG_BASE,
						bus, dev, func, 0);
				if ((devfuncHdr->vendorID == vendorID) &&
				    (devfuncHdr->deviceID == deviceID) &&
				    (index == count++)) {
					/* Match: return the matching PCI
					   Bus:Device:Function encoded
					   into an uint32. */
					info = encodePCIDevice(bus, dev,
							       func);
					return info;
				}
				/* Evaluate sub-functions only if the
				   device is multi-function */
				if (func == 0) {
					multifunction =
						devfuncHdr->headerType &
						PCI_HDR_TYPE_MULTIFCN;
					if (!multifunction) {
						break; /* Inner for */
					}
				}
			}
		}
	}
	/* The target device was not found. */
	return SYSERR;
}

/*------------------------------------------------------------------------
 * pci_read_config_byte  -  Read a byte from device's configuration space
 *------------------------------------------------------------------------
 */
int32 pci_read_config_byte(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int	offset,		/* Offset in config space	*/
		byte	*value		/* Pointer to store value	*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device, function nos.	*/
	void	*reg;		/* Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *((uint8 volatile *) reg);
	return OK;
}

/*------------------------------------------------------------------------
 * pci_read_config_word  -  Read a word from device's configuration space
 *------------------------------------------------------------------------
 */
int pci_read_config_word(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int32	offset,		/* Offset in config space	*/
		uint16	*value		/* Pointer to store value	*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device, function nos.	*/
	void	*reg;		/* Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint16 volatile *) reg;
	return OK;
}

/*------------------------------------------------------------------------
 * pci_read_config_dword  -  Read a dword from device's config space
 *------------------------------------------------------------------------
 */
int pci_read_config_dword(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int32	offset,		/* Offset in config space	*/
		uint32	*value		/* Pointer to store value	*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device, function nos.	*/
	void	*reg;		/* Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint32 volatile *) reg;
	return OK;
}

/*------------------------------------------------------------------------
 * pci_write_config_byte  -  Write a byte to device's config space
 *------------------------------------------------------------------------
 */
int pci_write_config_byte(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int32	offset,		/* Offset in config space	*/
		byte	value		/* Value to be written		*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device, function nos.	*/
	void	*reg;		/*Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint8 volatile *) reg = value;
	return OK;
}

/*------------------------------------------------------------------------
 * pci_write_config_word  -  Write a word to device's config space
 *------------------------------------------------------------------------
 */
int pci_write_config_word(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int32	offset,		/* Offset in config space	*/
		uint16	value		/* Value to be written		*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device, function nos.	*/
	void	*reg;		/* Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint16 volatile *) reg = value;
	return OK;
}

/*------------------------------------------------------------------------
 * pci_write_config_dword  -  Write a dword to device's config space
 *------------------------------------------------------------------------
 */
int pci_write_config_dword(
		uint32	encodedDev,	/* Encoded PCI device		*/
		int32	offset,		/* Offset in config space	*/
		uint32	value		/* Value to be written		*/
		)
{
	uint32 bus, dev, func;	/* PCI bus, device function nos.	*/
	void	*reg;		/* Register address			*/

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg = regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint32 volatile *) reg = value;
	return OK;
}

/*------------------------------------------------------------------------
 * pci_get_dev_mmio_base_addr  -  Get device's memory base address
 *------------------------------------------------------------------------
 */
int32 pci_get_dev_mmio_base_addr(
		uint32	encodedDev,	/* Encoded PCI device	*/
		int32	barIndex,	/* BAR index in config	*/
		void	**mmio_base_address
					/* MMIO base address	*/
		)
{
	uint32	bar_value;	/* BAR register value	*/
	int32	status;		/* Function call status	*/

	/* Determine the value of the target PCI device's MMIO base
	   address register */
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
	*mmio_base_address = (void *) (bar_value &
				       PCI_BAR_BASE_ADDRESS_MASK);
	return OK;
}

/*------------------------------------------------------------------------
 * pci_set_ivec  -  Set the interrupt vector for a device
 *------------------------------------------------------------------------
 */
int32	pci_set_ivec (
		uint32	pcidev,		/* Encoded PCI device	*/
		int32	inum,		/* Interrupt number	*/
		void	*handler,	/* Interrupt handler	*/
		int32	arg		/* Handler argument	*/
		)
{
	uint16	pci_ctrl, msi_ctrl;	/* PCI and MSI control reg val	*/
	byte	cap;			/* Capability byte		*/
	byte	next;			/* Next offset			*/

	/* Read the offset of first capability */

	pci_read_config_byte(pcidev, 0x34, &next);

	/* Look for the 'MSI' capability */

	while(next != 0) {

		pci_read_config_byte(pcidev, next, &cap);

		if(cap == 0x05) {
			break;
		}

		pci_read_config_byte(pcidev, next+1, &next);
	}

	/* If MSI capability not found, panic (for now) */

	if(next == 0) {
		panic("PCI device does not support MSI");
	}

	/* Set the interrupt handler for this device */

	set_ivec(inum, handler, arg);

	/* Turn off INTx emulation interrupts so MSI can work */

	pci_read_config_word(pcidev, 0x04, &pci_ctrl);
	pci_ctrl &= 0xFBFF;
	pci_write_config_word(pcidev, 0x04, pci_ctrl);

	/* Write the MSI address and data */

	pci_write_config_dword(pcidev, next+4, 0xFEE00000);
	pci_write_config_dword(pcidev, next+8, inum);

	/* Enable MSI */

	pci_read_config_word(pcidev, next+2, &msi_ctrl);
	msi_ctrl |= 0x0001;
	pci_write_config_word(pcidev, next+2, msi_ctrl);

	return OK;
}
