/* rdscontrol.c - rdscontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdscontrol  -  Provide control functions for the remote disk
 *------------------------------------------------------------------------
 */
devcall	rdscontrol (
	 struct dentry	*devptr,	/* Entry in device switch table	*/
	 int32	func,			/* The control function to use	*/
	 int32	arg1,			/* Argument #1			*/
	 int32	arg2			/* Argument #2			*/
	)
{
	struct	rdscblk	*rdptr;		/* Pointer to control block	*/
	struct	rdqnode	*rptr;		/* Pointer to a request node	*/
	struct	rd_msg_dreq msg;	/* Buffer for delete request	*/
	struct	rd_msg_dres resp;	/* Buffer for delete response	*/
	char	*to, *from;		/* Used during name copy	*/
	int32	retval;			/* Return value			*/
	uint32	mypid;			/* PID of the calling process	*/
	pri16	myprio;			/* Process priority		*/

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

	switch (func) {

	/* Synchronize writes */

	case RDS_CTL_SYNC:

		/* If request queue is empty, return immediately */

		if (rdptr->rd_qhead == (struct rdqnode *)NULL) {
			return OK;
		}

		/* Allocate a request node and fill in a sync request */

		rptr = rdptr->rd_qfree;
		rdptr->rd_qfree = rptr->rd_next;
		rptr->rd_op = RD_OP_SYNC;
		rptr->rd_blknum = 0;		/* unused */
		rptr->rd_callbuf = NULL;	/* unused */
		mypid = getpid();
		rptr->rd_pid = mypid;

		/* Insert the new request at the tail of the queue */

		rptr->rd_next = (struct rdqnode *)NULL;
		rptr->rd_prev = rdptr->rd_qtail;
		if (rdptr->rd_qtail == (struct rdqnode *)NULL) {
			/* Request queue was empty */
			rdptr->rd_qhead = rptr;
		}
		rdptr->rd_qtail = rptr;

		/* Atomically signal the comm. process semaphore and	*/
		/*   suspend the current process by temporarily setting	*/
		/*   the process  prirority to the highest possible	*/
		/*   value, performing the two actions, and then	*/
		/*   resetting the priority to its original value when	*/
		/*   the process is awakened				*/

		myprio = rdssetprio(MAXPRIO);
		signal(rdptr->rd_comsem);
		suspend(getpid());
		myprio = rdssetprio(myprio);
		return OK;

		/* Delete the remote disk (entirely remove it) */

	case RDS_CTL_DEL:

		/* Handcraft a message for the server that requests	*/
		/*	deleting the disk with the specified ID		*/

		msg.rd_type = htons(RD_MSG_DREQ);/* Request deletion	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;	/* rdscomm will insert sequence # later	*/
		to = msg.rd_id;
		memset(to, NULLCH, RD_IDLEN);	/* Initialize to zeroes	*/
		from = rdptr->rd_id;
		while ( (*to++ = *from++) != NULLCH ) {	/* copy ID	*/
			;
		}

		/* Send message and receive response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_dreq),
			 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_dres),
					rdptr);

		/* Check the response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
		     (ntohs(resp.rd_status) != 0) ) {
			return SYSERR;
		}
		return OK;

	default:
		kprintf("rsscontrol: invalid function %d\n");
		return SYSERR;
	}
	return OK;
}
