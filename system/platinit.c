/* platinit.c - platinit */
#include <xinu.h>

/*------------------------------------------------------------------
 * platinit - platform specific initialization for Galileo
 *------------------------------------------------------------------
 */
void	platinit()
{

	/* Remove Isolated Memory Region Protections */

	remove_irm_protections();

	/* Initialize the PCI bus */

	pci_init();

	/* Intel Quark Irq Routing */

	quark_irq_routing();

}
