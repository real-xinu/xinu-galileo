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
	struct	rdscblk	*rdptr;		/* Pointer to the control block	*/
					/*   for the disk device	*/
	struct	rdsent  *sptr;		/* Pointer tp serial queue entry*/
	struct	rd_msg_dreq msg;	/* Delete message to be sent	*/
	struct	rd_msg_dres resp;	/* Buffer to hold response	*/
	char	*to;			/* Used to copy the disk ID	*/
	char	*from;			/*   "   "   "   "    "   "	*/
	int32	retval;			/* Return value from rdscomm	*/


	/* If the device not currently open, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	switch (func) {

	    /* Synchronize writes */

	    case RDS_CTL_SYNC:

		/* If serial and request queues are empty, return */

		if ( (rdptr->rdscount == 0) && (rdptr->rd_qhead == 
						(struct rdqnode *)NULL)) {
			return OK;
		}

		/* Fill in the next serial queue entry */

		sptr = &rdptr->rd_sq[rdptr->rdstail++];
		if (rdptr->rdstail >= RD_SSIZE) {
			rdptr->rdstail = 0;
		}
		rdptr->rdscount++;
		sptr->rd_op = RD_OP_SYNC;
		sptr->rd_blknum = 0;	/* Not used */
		sptr->rd_pid = getpid();

		/* Atomically resume the communication process and	*/
		/*  suspend thef current process to wait until we	*/
		/*  recah the head of the request queue			*/

		rdsars(rdptr->rd_comproc);
		return RD_BLKSIZ;

	    /* Delete the remote disk (entirely remove it) */

	    case RDS_CTL_DEL:

		/* Handcraft a message for the server that requests	*/
		/*	deleting the disk with the specified ID		*/

		msg.rd_type = htons(RD_MSG_DREQ);/* Request deletion	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;	/* Rdscomm will insert sequence # later	*/
		to = msg.rd_id;
		memset(to, NULLCH, RD_IDLEN);	/* Initialize to zeroes	*/
		from = rdptr->rd_id;
		while ( (*to++ = *from++) != NULLCH ) {	/* Copy ID	*/
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
