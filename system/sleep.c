/* sleep.c - sleep sleepms */

#include <xinu.h>

#define	MAXSECONDS	4294967		/* Max seconds per 32-bit msec	*/

/*------------------------------------------------------------------------
 *  sleep  -  Delay the calling process n seconds
 *------------------------------------------------------------------------
 */
syscall	sleep(
	  uint32	delay		/* Time to delay in seconds	*/
	)
{
	if (delay > MAXSECONDS) {
		return SYSERR;
	}
	sleepms(1000*delay);
	return OK;
}

/*------------------------------------------------------------------------
 *  sleepms  -  Delay the calling process n milliseconds
 *------------------------------------------------------------------------
 */
syscall	sleepms(
	  uint32	delay		/* Time to delay in msec.	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
	if (delay == 0) {
		yield();
		restore(mask);
		return OK;
	}

	/* Delay calling process */

	if (insertd(currpid, sleepq, delay) == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	proctab[currpid].prstate = PR_SLEEP;
	resched();
	restore(mask);
	return OK;
}
