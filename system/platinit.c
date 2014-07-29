/* platinit.c - platinit */
#include <xinu.h>

/*------------------------------------------------------------------
 * platinit - platform specific initialization for Galileo
 *------------------------------------------------------------------
 */
void	platinit()
{

	int32	pcidev;

	/* Remove Isolated Memory Region Protections */

	remove_irm_protections();

	/* Initialize the PCI bus */

	pci_init();

	/* Intel Quark Irq Routing */

	quark_irq_routing();

	/* Find the UART device */
	pcidev = find_pci_device(INTEL_QUARK_UART_PCI_DID,
					INTEL_QUARK_UART_PCI_VID, 1);
	if(pcidev == SYSERR) {
		/* Read the base address for UART CSRs */
		pci_read_config_dword(pcidev, 0x10,
				(uint32 *)&devtab[CONSOLE].dvcsr);
	}

	pcidev = find_pci_device(INTEL_ETH_QUARK_PCI_DID,
					INTEL_ETH_QUARK_PCI_VID, 0);
	if(pcidev != SYSERR) {

		/* Store the pcidev for future use */
		ethertab[0].pcidev = pcidev;

		/* Read base address for Ethernet CSRs */
		pci_read_config_dword(pcidev, 0x10,
				(uint32 *)&devtab[ETHER0].dvcsr);
	}
}
