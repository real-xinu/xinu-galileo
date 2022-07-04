/* rdsread.c - rdsread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsread  -  Read a block from a remote disk
 *------------------------------------------------------------------------
 */
devcall	rdsread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer to hold disk block	*/
	  int32	blk			/* Block number of block to read*/
	)
{
	struct	rdscblk	*rdptr;		/* Pointer to the control block	*/
					/*   for the disk device	*/
	struct	rdqnode	*rptr;		/* Pointer that walks the	*/
					/*   request queue		*/
	struct	rdqnode	*tptr;		/* Pointer to tail node on the	*/
					/*   request queue		*/
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

	/* Search the cache for the specified block */

	cptr = rdptr->rd_chead;
	while (cptr != (struct rdcnode *)NULL) {
		if (cptr->rd_blknum == blk) {
			/* Satisfu the request */
			memcpy(buff, cptr->rd_data, RD_BLKSIZ);
			return RD_BLKSIZ;
		}
		cptr = cptr->rd_next;
	}

	/* Search backward in the request queue for the most recent	*/
	/*     occurrence of the block					*/

	rptr = rdptr->rd_qtail;  /* Tail of the requst queue */

	while (rptr != (struct rdqnode *)NULL) {
		if (rptr->rd_blknum != blk)  {
			rptr = rptr->rd_prev;
			continue;
		}

		/* Found a request for the same block */

		if (rptr->rd_op == RD_OP_WRITE) {
			/* Satisfy the reqeust */
			memcpy(buff, rptr->rd_callbuf, RD_BLKSIZ);
			return RD_BLKSIZ;
		} else {
			/* Read request */
			break;
		}
	}

	/* Allocate a request node and fill in a read request */

	rptr = rdptr->rd_qfree;
	rdptr->rd_qfree = rptr->rd_next;
	rptr->rd_op = RD_OP_READ;
	rptr->rd_blknum = blk;
	rptr->rd_callbuf = buff;
	rptr->rd_pid = getpid();

	/* Insert the new request at the tail of the queue */

	if (rdptr->rd_qhead == (struct rdqnode *)NULL) {
		/* Request queue is empty */
		rdptr->rd_qhead = rdptr->rd_qtail = rptr;
		rptr->rd_next = rptr->rd_prev =
			(struct rdqnode *)NULL;
	} else {
		tptr = rdptr->rd_qtail;
		tptr->rd_next = rptr;
		rptr->rd_prev = tptr;
		rptr->rd_next = (struct rdqnode *)NULL;
		rdptr->rd_qtail = rptr;
	}

	/* Atomically signal the comm. process semaphore and suspend	*/
	/*   the current process by temporarily setting the process	*/
	/*   prirority to the highest possible value, performing the	*/
	/*   two actions, and then resetting the priority to its	*/
	/*   original value when the process is awakened		*/

	myprio = 0xffff & rdssetprio(MAXPRIO);
	signal(rdptr->rd_comsem);
	suspend(getpid());
	rdssetprio(myprio);
	return RD_BLKSIZ;
}
