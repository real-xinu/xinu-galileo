/* rdsopen.c - rdsopen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsopen  -  Open a remote disk device and specify an ID to use
 *------------------------------------------------------------------------
 */

devcall	rdsopen (
	 struct	dentry	*devptr,	/* Entry in device switch table	*/
	 char	*diskid,		/* Disk ID to use		*/
	 char	*mode			/* Unused for a remote disk	*/
	)
{
	struct	rdscblk	*rdptr;		/* Ptr to control block entry	*/
	struct	rd_msg_oreq msg;	/* Message to be sent		*/
	struct	rd_msg_ores resp;	/* Buffer to hold response	*/
	int32	retval;			/* Return value from rdscomm	*/
	int32	len;			/* Counts chars in diskid	*/
	char	*idto;			/* Ptr to ID string copy	*/
	char	*idfrom;		/* Pointer into ID string	*/
	uint32	size;			/* Total size of memory needed	*/
					/*  for request or cache nodes	*/
	char	*p;			/* Ptr walks memory and links	*/
					/*  nodes on a list		*/
	char	*pend;			/* Address beyond the nodes	*/
	char	*pprev;			/* Address of previous buffer	*/
					/*  when linking them		*/

	/* Find the control block for this remote disk device */

	rdptr = &rdstab[devptr->dvminor];

	/* Reject if the device is already open */

	if (rdptr->rd_state != RD_CLOSED) {
		return SYSERR;
	}

	/* Prevent concurrent open attempts */

	rdptr->rd_state = RD_PEND;

	/* Copy the disk ID into the control block */

	idto = rdptr->rd_id;
	idfrom = diskid;
	len = 0;
	while ( (*idto++ = *idfrom++) != NULLCH) {
		len++;
		if (len >= RD_IDLEN) {	/* ID string is too long */
			rdptr->rd_state = RD_CLOSED;
			return SYSERR;
		}
	}

	/* Verify that the ID is non-null */

	if (len == 0) {
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;
	}

	/* Hand-craft an open request message to be sent to the server */

	msg.rd_type = htons(RD_MSG_OREQ);/* Request an open		*/
	msg.rd_status = htons(0);
	msg.rd_seq = 0;			/* Rdscomm fills in an entry	*/
	idto = msg.rd_id;
	memset(idto, NULLCH, RD_IDLEN);/* Initialize ID to zero bytes	*/

	idfrom = diskid;
	while ( (*idto++ = *idfrom++) != NULLCH ) { /* Copy ID to req.	*/
		;
	}

	/* Set the server port, local port, and	the server IP address.	*/

	if ( (int32)getlocalip() == SYSERR) {
		kprintf("RDS cannot obtain an IP address\n");
		return SYSERR;
	}
	rdptr->rd_ser_port = RD_SERVER_PORT;
	rdptr->rd_loc_port = RD_LOC_PORT + devptr->dvminor;
	if (dnslookup(RD_SERVER, &rdptr->rd_ser_ip) == SYSERR) {
		kprintf("rdsopen: lookup of %s failed\n", RD_SERVER);
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;
	}

	/* Send the open message and receive a response */
	retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_oreq),
			 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_ores),
				rdptr );

	/* Check the response */

	if ( (retval == SYSERR) || (retval == TIMEOUT) ||
	     (ntohs(resp.rd_status) != 0) ) {
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;
	}

	/* Initialize the serial queue to empty */

	rdptr->rdshead = rdptr->rdstail = rdptr->rdscount = 0;

	/* Initialize the request queue to empty */

	rdptr->rd_qhead = rdptr->rd_qtail = (struct rdqnode *) NULL;

	/* Allocate request queue nodes and link them onto a free list	*/

	size = sizeof(struct rdqnode) * RD_QNODES;

	p = getmem(size);
	if ((int32)p == SYSERR) {
		kprintf("rdsopen: cannot allocted request queue\n");
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;
	}
	rdptr->rd_qfree = (struct rdqnode *)p;
	pend = p + size;
	pprev = p; /* To prevent a Compiler initialization" warning */									
	while (p < pend) {	/* Walk through allocated memory */
		pprev = p;
		p = p + sizeof(struct rdqnode);
		((struct rdqnode *)pprev)->rd_next = (struct rdqnode *)p;
	}
	((struct rdqnode *)pprev)->rd_next = NULL;  /* End of the list	*/

	/* Initialize the cache to empty */

	rdptr->rd_chead = rdptr->rd_ctail = (struct rdcnode *) NULL;

	/* Allocate cache nodes and link them onto a cache free list	*/

	size = sizeof(struct rdcnode) * RD_CNODES;

	p = getmem(size);
	if ((int32)p == SYSERR) {
		kprintf("rdsopen: cannot allocate cache nodes\n");
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;		
	}
	rdptr->rd_cfree = (struct rdcnode *)p;
	pend = p + size;
	while (p < pend) {	/* Walk through allocated memory */
		pprev = p;
		p = p + sizeof(struct rdcnode);
		((struct rdcnode *)pprev)->rd_next = (struct rdcnode *)p;
	}
	((struct rdcnode *)pprev)->rd_next = NULL;  /* End of the list	*/

	/* Create the communication process for this remote disk */

	rdptr->rd_comproc = create(rdsprocess, RD_STACK, RD_PRIO,
						"remdisk", 1, rdptr);
	if (rdptr->rd_comproc == SYSERR) {
		kprintf("rdsopen: cannot create remote disk process");
		rdptr->rd_state = RD_CLOSED;
		return SYSERR;
	}

	/* Change state of device to open */

	rdptr->rd_state = RD_OPEN;

	/* Return the device ID of the remote disk device */

	return devptr->dvnum;
}
