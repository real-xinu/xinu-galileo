/* clkinit.c - clkinit */

#include <xinu.h>

uint32	clktime;		/* Seconds since boot			*/
uint32	ctr1000 = 0;		/* Milliseconds since boot		*/
qid16	sleepq;			/* Queue of sleeping processes		*/
uint32	preempt;		/* Preemption counter			*/
   
/*------------------------------------------------------------------------
 * clkinit  -  Initialize the clock and sleep queue at startup
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{
	uint16	intv;		/* Clock rate in KHz			*/

	/* Set interrupt vector for clock to invoke clkdisp */

	set_evec(IRQBASE, (uint32)clkdisp);

	/* Clock rate is 1.190 Mhz; this is 1 ms interrupt rate */

	intv = 1193;	/* Using 1193 instead of 1190 to fix clock skew	*/

	sleepq = newqueue();	/* Allocate a queue to hold the delta	*/
				/*  list of sleeping processes		*/
	preempt = QUANTUM;	/* Initial time quantum			*/

	/* Specify that seepq is initially empty */

	clktime = 0;		/* Start counting seconds		*/

	/* Set to: timer 0, 16-bit counter, rate generator mode,
				and counter is binary*/
	outb(CLKCNTL, 0x34);

	/* Must write LSB first, then MSB */
	outb(CLOCK0, (char) (0xff & intv) );
	outb(CLOCK0, (char) (0xff & (intv>>8)));
	return;
}
