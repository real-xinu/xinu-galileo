/* quark_irq.c - quark_irq_routing */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  quark_irq_routing  -  Set IRQ indirection for devices
 *------------------------------------------------------------------------
 */
int32	quark_irq_routing(void)
{
	int	pciinfo;		/* PCI info for Legacy bridge	*/
	uint32	rcba;			/* Root Complex Base Address	*/

	pciinfo = find_pci_device(INTEL_QUARK_LEGBR_PCI_DID,
					INTEL_QUARK_LEGBR_PCI_VID, 0);
	if(pciinfo == SYSERR) {
		return SYSERR;
	}

	/* Route PIRQ A-D to 8259 IRQ 2-5 respectively */

	pci_write_config_dword(pciinfo, PABCDRC_OFFSET, PABCDRC_VALUE);

	/* Route PIRQ E-H to 8259 IRQ 6-9 respectively */

	pci_write_config_dword(pciinfo, PEFGHRC_OFFSET, PEFGHRC_VALUE);

	/* Read the Root Complex Base Address */

	pci_read_config_dword(pciinfo, RCBA_OFFSET, &rcba);
	rcba &= RCBA_MASK;

	/* Route PCI interrupts to PIRQA-H, refer to quark_irq.h file
	 *   for specific IRQ assignments
	 */
	*(uint16 *)(rcba + IRQAGENT0_OFFSET) = IRQAGENT0_VALUE;
	*(uint16 *)(rcba + IRQAGENT1_OFFSET) = IRQAGENT1_VALUE;
	*(uint16 *)(rcba + IRQAGENT2_OFFSET) = IRQAGENT2_VALUE;
	*(uint16 *)(rcba + IRQAGENT3_OFFSET) = IRQAGENT3_VALUE;

	outb(0x4d1, 0x1E);
	return OK;
}
