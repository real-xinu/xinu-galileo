/* lflControl.c - lflControl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflControl - Provide control functions for a local file pseudo-device
 *------------------------------------------------------------------------
 */
devcall	lflControl (
	 struct dentry	*devptr,	/* entry in device switch table	*/
	 int32	func,			/* a control function		*/
	 int32	arg1,			/* argument #1			*/
	 int32	arg2			/* argument #2			*/
	)
{
	struct	lflcblk	*lfptr;		/* ptr to open file table entry	*/
	int32	retval;			/* return value from func. call	*/

	/* Obtain exclusive use of the file */

	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);

	/* If file is not open, return an error */

	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	switch (func) {

	/* Truncate a file */

	case LF_CTL_TRUNC:
		wait(Lf_data.lf_mutex);
		retval = lftruncate(lfptr);
		signal(Lf_data.lf_mutex);
		signal(lfptr->lfmutex);
		return retval;	

	default:
		kprintf("lfControl: function %d not valid\n\r", func);
		signal(lfptr->lfmutex);
		return SYSERR;
	}
}
