/* pipe_close.c  -  pipe_close */

#include <xinu.h>

/*------------------------------------------------------------------------
 * pipe_close  -  Close a pipe
 *------------------------------------------------------------------------
*/

devcall	pipe_close (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	pipecblk *piptr;	/* Pointer to pipe control block*/

	/* Note: because both a writing process and reading process use	*/
	/*  a given pipe, both will close the pipe.  Conceptually, the	*/
	/*  first call moves the pipe to a read-only state and marks the*/
	/*  end-of-ile.	The second call deallocates the pipe device,	*/
	/*  making it available for reuse.				*/

	piptr = &pipetab[devptr->dvminor];

	/* If pipe is completely closed, return SYSERR */

	if (piptr->pstate == PIPE_FREE) {
		return SYSERR;
	}

	/* First call to close -- move to EOF state */

	if (piptr->pstate == PIPE_OPEN) {
		piptr->pstate = PIPE_EOF;
		resched_cntl(DEFER_START);
		if (semcount(piptr->pcsem) < 0) {
			/* Pipe is empty and consumer is blocked, so	*/
			/* Allow the consumer to run   			*/
			semreset(piptr->pcsem, 0);
		}

		/* Allow a blocked producer to proceed, if any */

		if (semcount(piptr->ppsem) < 0) {
			semreset(piptr->ppsem, 0);
		}
		resched_cntl(DEFER_STOP);
		return OK;
	}

	/* Second call to close - deallocate the pipe device */

	piptr->pstate = PIPE_FREE;
	semdelete(piptr->ppsem);
	semdelete(piptr->pcsem);
	piptr->pavail = 0;
	return OK;
}
