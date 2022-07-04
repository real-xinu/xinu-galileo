/* rdsprocess.c - rdsprocess */

/*DEBUG*/ int	go = 1;

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsprocess  -  High-priority background process that repeatedly
 *		  extracts an item from the request queue, sends the
 *		  request to the remote disk server, and handles the
 *		  response, including caching responses blocks
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
	struct	rdqnode	*tptr;		/* Temp pointer to a node in	*/
					/*    the request queue		*/
	uint32	blk;			/* Block number in current req.	*/

	while (TRUE) {			/* Do forever */

	    /* Wait until a request arrives and node */

	    wait(rdptr->rd_comsem);
	    rptr = rdptr->rd_qhead;
	    if (rptr == (struct rdqnode *)NULL) {
		/* A request was satisfied early */
		continue;
	    }
	    blk = rptr->rd_blknum;

	    /* Use operation in request to determine action */

	   switch (rptr->rd_op) {

	   case RD_OP_SYNC:
		resume(rptr->rd_pid);
		rptr = rdqunlink(rdptr, rptr);
		break;

	   case RD_OP_READ:

		/* Build a read request message for the server */

		msg.rd_type = htons(RD_MSG_RREQ);	/* Read request	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;		/* rdscomm fills in the value	*/
		idto = msg.rd_id;
		memset(idto, NULLCH, RD_IDLEN);/* Initialize ID to zero	*/
		idfrom = rdptr->rd_id;
		while ( (*idto++ = *idfrom++) != NULLCH ) { /* Copy ID	*/
			;
		msg.rd_blk = htonl(blk);
		}

		/* Send the message and receive a response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_rreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_rres),
				  rdptr );

		/* Check response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			panic("remite disk error contacting server");
		}

		/* Copy data from the reply into the buffer */

		memcpy(rptr->rd_callbuf, resp.rd_data, RD_BLKSIZ);

		/* Resume the waiting process (will not run until the	*/
		/*     communication process blocks			*/

		resume(rptr->rd_pid);

		tptr = rdqunlink(rdptr, rptr);

		/* Walk the request queue and satisfy subsequent read	*/
		/*    requests for the same block			*/

		rptr = rdptr->rd_qhead;
		while (rptr != (struct rdqnode *)NULL) {
		    if (rptr->rd_blknum != blk) {
			rptr = rptr->rd_next;
			continue;
		    }
		    /* block number matches */
		    if (rptr->rd_op == RD_OP_WRITE) {
			/* Stop on a write for the block */
			break;
		    }
		    if ( rptr->rd_op == RD_OP_READ ) {
			/* Satisfy a subsequent read for the block */
			memcpy(rptr->rd_callbuf,resp.rd_data,RD_BLKSIZ);
			resume(rptr->rd_pid);
			rptr = rdqunlink(rdptr, rptr);
		    } else {
			rptr = rptr->rd_next;
		    }
		}
		break;

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
		msg.rd_blk = htonl(blk);
		memcpy(msg.rd_data, rptr->rd_callbuf, RD_BLKSIZ);

		/* Check request queue for subsequent writes of blk */

		tptr = rptr->rd_next;
		while (tptr != (struct rdqnode *)NULL) {
			if ( (tptr->rd_blknum == blk) &&
			     (tptr->rd_op == RD_OP_WRITE) ) {
				break;
			}
				
		}
		if (tptr == (struct rdqnode *)NULL) {
			/* No subsequent writes, so add to cache */
			rdcinsert(rdptr, blk, msg.rd_data);
		}

		/* Resume the process (comm. pocess continues to run) */

		resume(rptr->rd_pid);

		/* Unlink the node from the request queue */

		rptr = rdqunlink(rdptr, rptr);

		/* Send the message and receive a response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_wreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_wres),
				  rdptr );

		/* Check the response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			panic("remote disk access failed");
		}
		break;

	   default:
		break;/* SHould never happen */
	   }
	}
}
