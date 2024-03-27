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
	struct	rdsent  *sptr;		/* Pointer tp serial queue entry*/

	/* If the device not currently open, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Fill in the next serial queue entry */

	sptr = &rdptr->rd_sq[rdptr->rdstail++];
	if (rdptr->rdstail >= RD_SSIZE) {
		rdptr->rdstail = 0;
	}
	rdptr->rdscount++;
	sptr->rd_op = RD_OP_READ;
	sptr->rd_blknum = blk;
	sptr->rd_callbuf = buff;
	sptr->rd_pid = getpid();

	/* Atomically resume the communication process and suspend the	*/
	/*  the current process to wait for the read to complete	*/

	rdsars(rdptr->rd_comproc);
	return RD_BLKSIZ;
}
