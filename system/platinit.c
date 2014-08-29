/* platinit.c - platinit */
#include <xinu.h>

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
