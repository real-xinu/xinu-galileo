/* rdssetprio.c - rdssetprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  rdssetprio  -  Set the priority of the current process
 *------------------------------------------------------------------------
 */
pri16	rdssetprio(
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	prptr = &proctab[currpid];
	oldprio = prptr->prprio;
	prptr->prprio = newprio;
	resched();
	restore(mask);
	return oldprio;
}
