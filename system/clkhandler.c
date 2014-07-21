/* clkhandler.c - clkhandler */
#include <xinu.h>

/*---------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *---------------------------------------------------------------------------
 */
void	clkhandler()
{
	static	uint32	count1000 = 1000; /* local ms counter */

	/* Decrement the local ms counter */
	/* Check if 1 sec. has passed	  */

	if((--count1000) == 0) {

		/* Increment the sec. counter */

		clktime++;

		/* Reset the local ms counter to count the next sec. */

		count1000 = 1000;
	}

	/* Check if the sleepq is empty */

	if(!isempty(sleepq)) {

		/* Decrement the key of the first process on the sleepq */
		/* and check if we need to wakeup the process		*/

		if((--queuetab[firstid(sleepq)].qkey) == 0) {

			wakeup();
		}
	}

	/* Decrement the preemption counter	*/
	/* and check if it is time to reschedule*/

	if((--preempt) == 0) {

		resched();
	}
}
