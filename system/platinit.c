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
		return;
	}

	/* Read the base address for UART */
	pci_read_config_dword(pcidev, 0x10,
				(uint32 *)&devtab[CONSOLE].dvcsr);
}
