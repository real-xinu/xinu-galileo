/* lflInit.c  -  lflInit */

#include <xinu.h>

struct	lflcblk	lfltab[Nlfl];		/* control blocks */

/*------------------------------------------------------------------------
 * lflInit  -  initialize control blocks for local file pseudo-devices
 *------------------------------------------------------------------------
 */
devcall	lflInit (
	  struct dentry	*devptr		/* Entry in device switch table	*/
		)
{
	struct	lflcblk	*lfptr;		/* Ptr. to control block entry	*/
	int32	i;			/* Walks through name array	*/

	lfptr = &lfltab[ devptr->dvminor ];

	/* Initialize control block entry */

	lfptr->lfstate = LF_FREE;	/* Device is currently unused	*/
	lfptr->lfdev = devptr->dvnum;	/* Set device ID		*/
	lfptr->lfmutex = semcreate(1);
	lfptr->lfdirptr = (struct  ldentry *) NULL;
	lfptr->lfpos = 0;
	for (i=0; i<LF_NAME_LEN; i++) {
		lfptr->lfname[i] = NULLCH;
	}
	lfptr->lfinum = LF_INULL;
	memset((char *) &lfptr->lfiblock, NULLCH, sizeof(struct lfiblk));
	lfptr->lfdnum = 0;
	memset((char *) &lfptr->lfdblock, NULLCH, LF_BLKSIZ);
	lfptr->lfbyte = &lfptr->lfdblock[LF_BLKSIZ]; /* beyond lfdblock	*/
	lfptr->lfibdirty = lfptr->lfdbdirty = FALSE;
	return OK;
}
