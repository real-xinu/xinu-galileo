/* rdsprocess.c - rdsprocess */
#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsprocess  -  High-priority background process that repeatedly
 *		   moves items from the serial queue to the request queue
 *		   extracts the next item on the request queue
 *		   sends the request to the remote disk server, and
 *		   caches new blocks
 *------------------------------------------------------------------------
 */

void	rdsprocess (
	  struct rdscblk    *rdptr	/* Ptr to device control block	*/
	)
{
	struct	rd_msg_wreq msg;	/* Message to be sent		*/
					/*   (includes data area)	*/
	struct	rd_msg_rres resp;	/* Buffer to hold response	*/
					/*   (includes data area)	*/
	int32	retval;			/* Return value from rdscomm	*/
	char	*idto;			/* Ptr to ID string for copy	*/
	char	*idfrom;		/* Ptr into ID string for copu	*/
	struct	rdqnode	*rptr;		/* Ptr to a node in the request	*/
					/*    queue			*/
	struct	rdsent	*sptr;		/* Ptr to item in serial queue	*/
	bool8	wfound;			/* Was a write found in the q?	*/


	while (TRUE) {			/* Do forever */

	    /* Move entries from the serial queue to the request queue	*/
	    /*  as long as request queue nodes are available		*/

	    while (rdptr->rdscount > 0) {
		sptr = &rdptr->rd_sq[rdptr->rdshead];

		/* If read can be satisfied from cache, satisfy it */

		if ( (sptr->rd_op == RD_OP_READ) && 
		     (rdcget(rdptr, sptr->rd_blknum, sptr->rd_callbuf)
								== OK)) {
			rdptr->rdscount--;
			rdptr->rdshead++;
			if (rdptr->rdshead >= RD_SSIZE) {
				rdptr->rdshead = 0;
			}
			resume(sptr->rd_pid);
			continue;
		}

		/* If no request queue nodes available, stop moving	*/
		/*  items from the serial queue to the request queue	*/

		if ((rptr = rdptr->rd_qfree) == (struct rdqnode *)NULL) {
			break;
		}

		/* Decrement count of nodes in the serial queue */

		rdptr->rdscount--;

		/* Remove the next request queue node from the free list*/

		rdptr->rd_qfree = rptr->rd_next;

		/* Fill in the blknum and pid from the serial node	*/

		rptr->rd_blknum = sptr->rd_blknum;
		rptr->rd_pid = sptr->rd_pid;

		/* Pass buffer pointer from serial to request queue node*/
		rptr->rd_callbuf = sptr->rd_callbuf;

		/* Use the operation to determine remaining actions */

		if ( (rptr->rd_op = sptr->rd_op) == RD_OP_WRITE) {
		    /* Copy data from the caller's buffer */
		    memcpy(rptr->rd_wbuf, sptr->rd_callbuf, RD_BLKSIZ);
		    /* Add the block to the cache */
		    rdcadd(rdptr, rptr->rd_blknum, rptr->rd_wbuf); 
		    /* Resume the process */
		    resume(rptr->rd_pid);
		}
		/* Add the request node to the request queue */
		rdqadd(rdptr, rptr);

		/* Increment the serial queue */
		rdptr->rdshead++;
		if (rdptr->rdshead >= RD_SSIZE) {
			rdptr->rdshead = 0;
		}

	    }

	    /* At this point, eligible items have been moved from the	*/
	    /*  serial queue to the request queue -- either suspend the	*/
	    /*  communication process, if the request queue is empty,	*/
	    /*  or handle the next request.				*/

	    if ( (rptr=rdptr->rd_qhead) == (struct rdqnode *)NULL ) {
			suspend(getpid());
			continue;
	    }

	    /* Handle a request -- use operation to determine action	*/
	    switch (rptr->rd_op) {

	    case RD_OP_SYNC:
		/* Resume the process and free the request queue node	*/
		resume(rptr->rd_pid);
		rptr = rdqfree(rdptr, rptr);
		continue;

	    case RD_OP_WRITE:
		/* Build a write request message for the server */

		msg.rd_type = htons(RD_MSG_WREQ);	/* Write request*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;		/* Rdscomb fills in an entry	*/
		idto = msg.rd_id;
		memset(idto, NULLCH, RD_IDLEN);/* Initialize ID to zero	*/
		idfrom = rdptr->rd_id;
		while ( (*idto++ = *idfrom++) != NULLCH ) { /* Copy ID	*/
			;
		}
		msg.rd_blk = htonl(rptr->rd_blknum);
		memcpy(msg.rd_data, rptr->rd_callbuf, RD_BLKSIZ);

		/* Free the node that was on the request queue */

		rptr = rdqfree(rdptr, rptr);

		/* Send the message and receive a response */
		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_wreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_wres),
				  rdptr );

		/* Check the response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			kprintf("remote disk write failed\n");
		}
		continue;

	    case RD_OP_READ:
		/* Build a read request message for the server */
		msg.rd_type = htons(RD_MSG_RREQ);	/* Read request	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;		/*  rdscomm fills in the value	*/
		idto = msg.rd_id;
		memset(idto, NULLCH, RD_IDLEN);/* Initialize ID to zero	*/
		idfrom = rdptr->rd_id;
		while ( (*idto++ = *idfrom++) != NULLCH ) { /* Copy ID	*/
			;
		}
		msg.rd_blk = htonl(rptr->rd_blknum);

		/* Send the message and receive a response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_rreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_rres),
				  rdptr );

		/* Check the response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			panic("remote disk error contacting server");
		}

		/* Walk through the request queue and satisfy all read 	*/
		/*   requests for the block, including the first one	*/
		wfound = FALSE;
		rptr = rdptr->rd_qhead;
		while (rptr != (struct rdqnode *)NULL) {
		    if (rptr->rd_blknum != rptr->rd_blknum) {
			rptr = rptr->rd_next;
			continue;
		    }
		    /* Block number matches; handle read or write */
		    if (rptr->rd_op == RD_OP_WRITE) {
			wfound = TRUE;
			break;
		    }
		    if ( rptr->rd_op == RD_OP_READ ) {
			/* Satisfy the read for the block */
			memcpy(rptr->rd_callbuf, resp.rd_data, RD_BLKSIZ);
			resume(rptr->rd_pid);
			rptr = rdqfree(rdptr, rptr);
		    }
		}

		/* Cache the block if no sucessive writes have appeared */

		if (! wfound) {
			rdcadd(rdptr, htonl(resp.rd_blk), resp.rd_data);
		}
		continue;

	   default:
		continue; /* Should never happen */
	    }
	}
}
