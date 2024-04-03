/* rdsars.c - rdsars */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  rdsars - atomically resume a high-priority server process and suspend
 *		the current process
 *------------------------------------------------------------------------
 */
syscall	rdsars(
	  pid32		pid		/* ID of the process to resume	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	/* Set current process state to suspended */
	proctab[currpid].prstate = PR_SUSP;

	/* If target process is suspended, resume it */

	prptr = &proctab[pid];
	if (prptr->prstate == PR_SUSP) {
		ready(pid);
	} else {
		resched();
	}
	restore(mask);
	return OK;
}
