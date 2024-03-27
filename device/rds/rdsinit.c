/* rdsinit.c - rdsinit */

#include <xinu.h>

struct	rdscblk	rdstab[Nrds];

/*------------------------------------------------------------------------
 *  rdsinit  -  Initialize the remote disk system device
 *------------------------------------------------------------------------
 */
devcall	rdsinit (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	rdscblk	*rdptr;		/* Ptr to device contol block	*/

	/* Obtain the address of the control block */

	rdptr = &rdstab[devptr->dvminor];

	/* Set the device to the closed state */

	rdptr->rd_state = RD_CLOSED;

	/* Set initial message sequence number */

	rdptr->rd_seq = 1;

	rdptr->rd_id[0] = NULLCH;

	/* Specify that the server UDP port is not yet registered */

	rdptr->rd_registered = FALSE;

	return OK;
}
