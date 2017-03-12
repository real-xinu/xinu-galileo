/* clkinit.c - clkinit (x86) */

#include <xinu.h>

uint32	clktime;		/* Seconds since boot			*/
uint32	count1000;		/* Milliseconds since last clock tick   */
qid16	sleepq;			/* Queue of sleeping processes		*/
uint32	preempt;		/* Preemption counter			*/

volatile struct hpet_csreg *hpet = (struct hpet_csreg *)
						HPET_BASE_ADDR;

/*------------------------------------------------------------------------
 * clkinit  -  Initialize the clock and sleep queue at startup (x86)
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{
	/* Allocate a queue to hold the delta list of sleeping processes*/

	sleepq = newqueue();

	/* Initialize the preemption count */

	preempt = QUANTUM;

	/* Initialize the time since boot to zero */

	clktime = 0;
        count1000 = 0;
	/* Set interrupt vector for the clock to invoke clkdisp */

	ioapic_irq2vec(2, IRQBASE);

	set_ivec(IRQBASE, clkhandler, 0);

	hpet->gc = 0;

	hpet->mcv_l = 0;
	hpet->mcv_u = 0;

	hpet->t0cc_l |= HPET_TXCC_TVS;
	hpet->t0cv_l = 14318;
	hpet->t0cc_l |= HPET_TXCC_TVS;
	hpet->t0cv_u = 0;

	hpet->t0cc_l = HPET_TXCC_IT | HPET_TXCC_TYP | HPET_TXCC_IE;

	hpet->gc = HPET_GC_OE;

	return;
}
