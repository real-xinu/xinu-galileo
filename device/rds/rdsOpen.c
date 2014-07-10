/* rdsOpen.c  -  rdsOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsOpen - open a remote disk device and specify an ID to use
 *------------------------------------------------------------------------
 */

devcall	rdsOpen (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 char	*diskid,		/* disk ID to use		*/
	 char	*mode			/* unused for a remote disk	*/
	)
{
	struct	rdscblk	*rdptr;		/* ptr to control block entry	*/
	struct	rd_msg_oreq msg;	/* message to be sent		*/
	struct	rd_msg_ores resp;	/* buffer to hold response	*/
	int32	retval;			/* return value from rdscomm	*/
	int32	len;			/* counts chars in diskid	*/
	char	*idto;			/* ptr to ID string copy	*/
	char	*idfrom;		/* pointer into ID string	*/

	rdptr = &rdstab[devptr->dvminor];

	/* Reject if device is already open */

	if (rdptr->rd_state != RD_FREE) {
		return SYSERR;
	}
	rdptr->rd_state = RD_PEND;

	/* Copy disk ID into free table slot */

	idto = rdptr->rd_id;
	idfrom = diskid;
	len = 0;
	while ( (*idto++ = *idfrom++) != NULLCH) {
		len++;
		if (len >= RD_IDLEN) {	/* ID string is too long */
			return SYSERR;
		}
	}

	/* Verify that name is non-null */

	if (len == 0) {
		return SYSERR;
	}

	/* Hand-craft an open request message to be sent to the server */

	msg.rd_type = htons(RD_MSG_OREQ);/* Request an open		*/
	msg.rd_status = htons(0);
	msg.rd_seq = 0;			/* rdscomm fills in an entry	*/
	idto = msg.rd_id;
	memset(idto, NULLCH, RD_IDLEN);/* initialize ID to zero bytes	*/

	idfrom = diskid;
	while ( (*idto++ = *idfrom++) != NULLCH ) { /* copy ID to req.	*/
		;
	}

	/* Send message and receive response */

	retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_oreq),
			 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_ores),
				rdptr );

	/* Check response */

	if (retval == SYSERR) {
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file open\n\r");
		return SYSERR;
	} else if (ntohs(resp.rd_status) != 0) {
		return SYSERR;
	}

	/* Change state of device to indicate currently open */

	rdptr->rd_state = RD_OPEN;

	/* Return device descriptor */

	return devptr->dvnum;
}
