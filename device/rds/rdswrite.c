/* rdswrite.c - rdswrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdswrite  -  Write a block to a remote disk
 *------------------------------------------------------------------------
 */
devcall	rdswrite (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer that holds a disk blk	*/
	  int32	blk			/* Block number to write	*/
	)
{
	struct	rdscblk	*rdptr;		/* Pointer to the control block	*/
					/*   for the disk device	*/
	struct	rdqnode	*rptr;		/* Pointer that walks the	*/
					/*   request list		*/
	struct	rdcnode	*cptr;		/* Pointer that walks the cache	*/
	pri16	myprio;			/* Temp storage for my priority	*/

	/* If the device not currently open, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Ensure the communication process is runnning */

	if ( ! rdptr->rd_comruns ) {
		rdptr->rd_comruns = TRUE;
		resume(rdptr->rd_comproc);
	}


	/* If block is present in the cache, remove it and return the	*/
	/*   node to the free list					*/

	cptr = rdptr->rd_chead;
	while (cptr != (struct rdcnode *)NULL) {
	    if (cptr->rd_blknum == blk) {
		rdcunlink(rdptr, cptr);
		break;
	    }
	    cptr = cptr->rd_next;
	}

	/* Allocate a request node and fill in a write request */

	rptr = rdptr->rd_qfree;
	rdptr->rd_qfree = rptr->rd_next;
	rptr->rd_op = RD_OP_WRITE;
	rptr->rd_blknum = blk;
	rptr->rd_callbuf = buff;
	rptr->rd_pid = getpid();

	/* Insert the new request at the tail of the queue */

	rdqinsert(rdptr, rptr);

	/* Atomically signal the comm. process semaphore and suspend	*/
	/*   the current process by temporarily setting the process	*/
	/*   priority to the highest possible value, performing the	*/
	/*   two actions, and then resetting the priority to its	*/
	/*   original value when the process is awakened		*/

	myprio = rdssetprio(MAXPRIO);
	signal(rdptr->rd_comsem);
	suspend(getpid());
	rdssetprio(myprio);
	return OK;
}
