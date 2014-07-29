/* lflSeek.c  -  lfSeek */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lfseek  -  seek to a specified position in a file
 *------------------------------------------------------------------------
 */
devcall	lflSeek (
	  struct dentry *devptr,	/* entry in device switch table */
	  uint32	offset		/* byte position in the file	*/
	)
{
	struct	lflcblk	*lfptr;		/* ptr to open file table entry	*/

	/* If file is not open, return an error */

	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);
	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	/* Verify offset is within current file size */

	if (offset > lfptr->lfdirptr->ld_size) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	/* Record new offset and invalidate byte pointer (i.e.,	*/
	/*   force the index and data blocks to be replaced if	*/
	/*   an attempt is made to read or write)		*/

	lfptr->lfpos = offset;
	lfptr->lfbyte = &lfptr->lfdblock[LF_BLKSIZ];

	signal(lfptr->lfmutex);
	return OK;
}
