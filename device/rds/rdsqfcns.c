/* rdsqfcns.c - rdqadd, rdqfree, rdcget, rdcadd, rdcdump, rdqdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdqadd  -  Add a request node to the tail of the request queue
 *------------------------------------------------------------------------
 */
void rdqadd (
	struct rdscblk *rdptr,		/* Control block pointer	*/
	struct rdqnode *rptr		/* Ptr to the request node	*/
) {
	struct rdqnode *tptr;		/* Pointer to the tail		*/

	tptr = rdptr->rd_qtail;
        rptr->rd_next = (struct rdqnode *)NULL;
        rptr->rd_prev = tptr;
        if (tptr == (struct rdqnode *)NULL) {
                /* Request queue was empty */
                rdptr->rd_qhead = rdptr->rd_qtail = rptr;
        } else {
                tptr->rd_next = rdptr->rd_qtail = rptr;
        }
	return;
}

/*------------------------------------------------------------------------
 * rdqfree  -	Unlink a node from the request queue, place the node on
 *		the free list, and return the address of the next node
 *		on the request queue (or NULL, if unlinking the tail)
 *------------------------------------------------------------------------
 */
struct rdqnode * rdqfree(
	struct rdscblk *rdptr,		/* Control block pointer	*/
	struct rdqnode *rptr		/* Ptr to the request node	*/
) {
	struct rdqnode *nptr;		/* Pointer to next node		*/
	struct rdqnode *pptr;		/* Pointer to previous node	*/

	nptr = rptr->rd_next;		/* Point to next node or NULL	*/
	pptr = rptr->rd_prev;		/* Point to prev. node or NULL	*/

	/* Handle unlinking the head of the request queue */

	if (rptr == rdptr->rd_qhead) {
		/* See if queue contains multiple nodes */
		if (rptr == rdptr->rd_qtail) {
			/* This is the only node on the queue */
			rdptr->rd_qhead = rdptr->rd_qtail =
				(struct rdqnode *)NULL;
		} else {
			/* The queue contains other nodes */
			rdptr->rd_qhead = nptr;
		}

	/* Handle unlinking the tail of the request queue */

	} else if (rptr == rdptr->rd_qtail) {
		rdptr->rd_qtail = pptr;
		pptr->rd_next = (struct rdqnode *)NULL;

	/* Handle unlinking a node in the middle of the queue */

	} else {		
		pptr = rptr->rd_prev;
		pptr->rd_next = nptr;
		nptr->rd_prev = pptr;
	}
	/* Add the unlinked node to free list */
	rptr->rd_next = rdptr->rd_qfree;
	rdptr->rd_qfree = rptr;
	return nptr;
}

/*------------------------------------------------------------------------
 * rdsget   -	Search the cache for a block and, if found, copy the
 *			data into the caller's buffer
 *------------------------------------------------------------------------
 */
int32	rdcget (
	    struct rdscblk *rdptr,	/* Remote disk control block	*/
	    uint32 blknum,		/* Block number to use		*/
	    char *buf			/* Buffer to receive the data	*/
) {
	struct rdcnode *cptr;		/* Pointer that walks the cache	*/

	if (RD_CNODES == 0) {
		/* No cache is being used */
		return SYSERR;
	}

	for (cptr = rdptr->rd_chead; cptr != (struct rdcnode*)NULL;
					cptr = cptr->rd_next) {
		if (cptr->rd_blknum != blknum) {
			continue;
		}
		/* Found the block in the cache -- copy the data */
		memcpy(buf, cptr->rd_data, RD_BLKSIZ);
		return OK;
	}
	return SYSERR;
}

/*------------------------------------------------------------------------
 * rdcadd  -	Add a cache entry for a specified block and data,
 *		 replacing an existing entry, allocting a free noode, or
 *		 stealing the oldest node the cache
 *------------------------------------------------------------------------
 */
void rdcadd (
	  struct rdscblk *rdptr,	/* Remote disk control block	*/
	  uint32 blk,			/* Block number to use		*/
	  char *buf			/* Data for the block		*/
) {
	struct rdcnode *cptr;		/* Pointer that walks the cache	*/
	struct rdcnode *tptr;		/* Temp. pointer to cache node	*/
					/*    used when unlinking	*/

	if (RD_CNODES == 0) {
		/* No cache is being used */
		return;
	}

	/* Replace previous entry for the block, if one exists */

	for (cptr = rdptr->rd_chead; cptr != (struct rdcnode*)NULL;
					cptr = cptr->rd_next) {
		if (cptr->rd_blknum != blk) {
			continue;
		}
		/* Found the block in the cache -- replace the  data	*/
		/*   witht he new data					*/
		memcpy(cptr->rd_data, buf, RD_BLKSIZ);
		return;
	}
	
	/* Allocate a free cache node, if any are available, or steal	*/
	/*	the oldest node in the cache				*/

	cptr = rdptr->rd_cfree;
	if (cptr != (struct rdcnode *)NULL) {
		/* Unlink from the free list */
		rdptr->rd_cfree = cptr->rd_next;
	} else {
		/* Steal the tail of the cache list */
		cptr = rdptr->rd_ctail;
		if (rdptr->rd_chead == rdptr->rd_ctail) {
			/* Exactly one node in the cache, so unlink it */
			rdptr->rd_chead = rdptr->rd_ctail =
				(struct rdcnode *)NULL;
		} else {
			/* More than one node in the cache */
			tptr = cptr->rd_prev;
			tptr->rd_next = (struct rdcnode *)NULL;
			rdptr->rd_ctail = tptr;			
		}
	}
	/* Fill in the block to be cached */
	cptr->rd_blknum = blk;
	memcpy(cptr->rd_data, buf, RD_BLKSIZ);

	/* Add the node to head of the cache */

	tptr = rdptr->rd_chead;
	cptr->rd_next = tptr;
	cptr->rd_prev = (struct rdcnode *)NULL;
	rdptr->rd_chead = cptr;
	if (tptr == (struct rdcnode *)NULL) {
		/* Insert in an empty cache */
		rdptr->rd_chead = rdptr->rd_ctail = cptr;
	} else {
		tptr->rd_prev = cptr;
	}
	return;
}

/*------------------------------------------------------------------------
 * rdqdump  -  Dump the remote disk request queue
 *------------------------------------------------------------------------
 */
void rdqdump (
	  did32 disk		/* ID of disk to use */
	)
{
	struct	dentry	*devptr;	/* Entry in Device Switch Table	*/
	struct	rdscblk	*rdptr;		/* Pointer to the control block	*/
	struct	rdqnode *rptr;		/* Ptr to item in request queue	*/
	int32	n;			/* Request number		*/
	int32	i;			/* Byte index in data		*/
	byte	ch;			/* One byte of data		*/

	devptr = (struct dentry *) &devtab[disk];
	rdptr = &rdstab[devptr->dvminor];
	rptr = rdptr->rd_qhead;

	if (rptr == (struct rdqnode *)NULL) {
		kprintf("****** request queue is empty ******\n");
		return;
	} else {
		kprintf("****** request queue ******\n");
	}

	n = 0;
	while (rptr != (struct rdqnode *)NULL) {
		n++;
		kprintf("Request Number %d ", n);
		kprintf("OP=%d  ", rptr->rd_op);
		kprintf("Blk=%d  ", rptr->rd_blknum);
		kprintf("PID=%d\n", rptr->rd_pid);
		if (rptr->rd_op == RD_OP_WRITE) {
		    for (i = 0; i < RD_BLKSIZ; i++) {
			ch = rptr->rd_callbuf[i];
			if ( (ch >= 'a' && ch <= 'z') ||
			     (ch >= 'A' && ch <= 'Z') ||
			     (ch >= '0' && ch <= '9') ||
			      ch == ' ' || ch == '.'  ||
			      ch == '-' || ch == '+'  ||
			      ch == '*' || ch=='\n' )  {
				kprintf("%c", ch);
			} else {
				kprintf("?");
			}
			if ( (i%512) == 0) {
				kprintf("\n");
			}
			if ( (i%64) == 0) {
				kprintf("\n");
			}
		    }
		    kprintf("\n**** END OF DATA ****\n");
		}
		rptr = rptr->rd_next;
	}
}

/*------------------------------------------------------------------------
 * rdcdump  -  Dump the remote disk cache
 *------------------------------------------------------------------------
 */
void rdcdump (
	  did32 disk		/* ID of disk to use */
	)
{
	struct	dentry	*devptr;	/* Entry in Device Switch Table	*/
	struct	rdscblk	*rdptr;		/* Pointer to the control block	*/
	struct	rdcnode *cptr;		/* Ptr to item in request queue	*/
	int32	n;			/* Node number		*/
	int32	i;			/* Byte index in data		*/
	byte	ch;			/* One byte of data		*/

	devptr = (struct dentry *) &devtab[disk];
	 rdptr = &rdstab[devptr->dvminor];
	cptr = rdptr->rd_chead;

	if (cptr == (struct rdcnode *)NULL) {
		kprintf("****** disk cache is empty ******\n");
		return;
	} else {
		kprintf("****** disk cache ******\n");
	}
	n = 0;
	while (cptr != (struct rdcnode *)NULL) {
		n++;
		kprintf("\nNode %2d    blk%4d\n", n, cptr->rd_blknum);
		for (i = 0; i < RD_BLKSIZ; i++) {
			ch = 0xff & cptr->rd_data[i];
			if ( (ch >= 'a' && ch <= 'z') ||
			     (ch >= 'A' && ch <= 'Z') ||
			     (ch >= '0' && ch <= '9') ||
			      ch == ' ' || ch == '.'  ||
			      ch == '-' || ch == '+'  ||
			      ch == '*' || ch=='\n' )  {
				kprintf("%c", ch);
			} else {
				kprintf("?");
			}
			if ( (i%512) == 0) {
				kprintf("\n");
			}
			if ( (i%64) == 0) {
				kprintf("\n");
			}
		}
		kprintf("\n**** END OF DATA ****\n");
		cptr = cptr->rd_next;
	}
	return;
}
