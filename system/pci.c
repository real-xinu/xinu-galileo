/* pci.c - pci_init */

#include <xinu.h>

#define MMCONFIG_BASE		0xe0000000	// teshack temporary
#define MMCONFIG_LEN		0x10000000	// teshack temporary

/*------------------------------------------------------------------------
 * pci_init  -  Initialize the PCI bios structures
 *------------------------------------------------------------------------
 */
int32	pci_init(void)
{
//kprintf("%s(): using PCI MMConfig space at address 0x%x, length 0x%x\n", __FUNCTION__, MMCONFIG_BASE, MMCONFIG_LEN);
	return OK;
}

/*------------------------------------------------------------------------
 * regAddress_MMConfig  -  Get the address of a device in MMConfig space
 *------------------------------------------------------------------------
 */
static inline void *
regAddress_MMConfig(
	unsigned int	mmconfigBase,
	unsigned int	bus,
	unsigned int	device,
	unsigned int	function,
	unsigned int    offset
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
encodePCIDevice(
	unsigned int	bus,
	unsigned int	dev,
	unsigned int	func
)
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
encodedPCIDevToBus(
	unsigned int	encodedDev
)
{
	return ((encodedDev >> PCI_BUS_SHIFT) & PCI_BUS_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToDevice  -  Extract device from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodedPCIDevToDevice(
	unsigned int	encodedDev
)
{
	return ((encodedDev >> PCI_DEV_SHIFT) & PCI_DEV_MASK);
}

/*------------------------------------------------------------------------
 * encodedPCIDevToFunction - Extract func from encoded bus/device/func
 *------------------------------------------------------------------------
 */
static inline unsigned int
encodedPCIDevToFunction(
	unsigned int	encodedDev
)
{
	return encodedDev & PCI_FUNC_MASK;
}

/*------------------------------------------------------------------------
 * find_pci_device - find a specified PCI device on the bus
 *------------------------------------------------------------------------
 */
int find_pci_device(int deviceID, int vendorID, int index)
{
	int		count;
	unsigned int	info;
	unsigned int	bus, dev, func;
	struct pci_config_header	*devfuncHdr;

	// Traverse memory-mapped PCI configuration space looking for
	// a match against the target device.
	count = 0;
	for (bus = 0; bus < PCI_MAX_BUSES; bus++) {
		for (dev = 0; dev < PCI_DEVICES_PER_BUS; dev++) {
			for (func = 0; func < PCI_FUNCTIONS_PER_DEVICE; func++) {
				devfuncHdr =
					regAddress_MMConfig(
						MMCONFIG_BASE,
						bus, dev, func, 0);
				if ((devfuncHdr->vendorID == vendorID) &&
				    (devfuncHdr->deviceID == deviceID)) {
					if (index == count++) {
						// Match: return the matching PCI Bus:Device:Function
						// encoded into an unsigned int.
						info = encodePCIDevice(bus, dev, func);
						return info;
					}
				}
				// Evaluate sub-functions only if the device is multi-function
				if (func == 0 &&
				    ((devfuncHdr->headerType & PCI_HDR_TYPE_MULTIFUNCTION) == 0)) {
					break; // Inner for loop.
				}
			}
		}
	}
	// The target device was not found.
	return SYSERR;
}


int pci_read_config_byte(uint32 encodedDev, int offset, unsigned char *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *((uint8 volatile *) reg);
	return 0;
}

int pci_read_config_word(uint32 encodedDev, int offset, unsigned short *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint16 volatile *) reg;
	return 0;
}

int pci_read_config_dword(uint32 encodedDev, int offset, uint32 *value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*value = *(uint32 volatile *) reg;
	return 0;
}

int pci_write_config_byte(uint32 encodedDev, int offset, unsigned char value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint8 volatile *) reg = value;
	return 0;
}

int pci_write_config_word(uint32 encodedDev, int offset, unsigned short value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint16 volatile *) reg = value;
	return 0;
}

int pci_write_config_dword(uint32 encodedDev, int offset, uint32 value)
{
	unsigned int bus, dev, func;
	void	*reg;

	bus = encodedPCIDevToBus(encodedDev);
	dev = encodedPCIDevToDevice(encodedDev);
	func = encodedPCIDevToFunction(encodedDev);
	reg =  regAddress_MMConfig(MMCONFIG_BASE, bus, dev, func, offset);
	*(uint32 volatile *) reg = value;
	return 0;
}
