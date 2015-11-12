/* platinit.c - platinit */
#include <xinu.h>

#define QUARK_CONS_PORT		1		/* Index of console UART */
#define QUARK_CONS_BAR_INDEX	0		/* Index of console's MMIO
						   base address register */
/*------------------------------------------------------------------
 * console_init - initialize the serial console.  The serial console
 * is on the second memory-mapped 16550 UART device.
 *------------------------------------------------------------------
 */
int console_init(void)
{
	int	status;
	int	pciDev;

	pciDev = find_pci_device(INTEL_QUARK_UART_PCI_DID,
				 INTEL_QUARK_UART_PCI_VID,
				 QUARK_CONS_PORT);
	if (pciDev < 0) {
		/* Error finding console device */
		return	pciDev;
	}
	/* Store the console device CSR base address into the console device's
	   device table entry. */
	status = pci_get_dev_mmio_base_addr(pciDev, QUARK_CONS_BAR_INDEX,
					   &devtab[CONSOLE].dvcsr);
	return status;
}

/*------------------------------------------------------------------
 * platinit - platform specific initialization for Galileo
 *------------------------------------------------------------------
 */
void	platinit()
{

	int32	pcidev;

	/* Initialize the PCI bus */
	pci_init();

	/* Initialize the console serial port */
	console_init();

	/* Remove Isolated Memory Region Protections */
	remove_irm_protections();

	/* Intel Quark Irq Routing */
	quark_irq_routing();

	/* Find the Ethernet device */
	pcidev = find_pci_device(INTEL_ETH_QUARK_PCI_DID,
					INTEL_ETH_QUARK_PCI_VID, 0);
	if(pcidev != SYSERR) {

		/* Store the pcidev for future use */
		ethertab[0].pcidev = pcidev;

		pci_get_dev_mmio_base_addr(pcidev, 0,
					  &devtab[ETHER0].dvcsr);
	}
}
