/* lflPutc.c  -  lfPutc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflPutc  -  write a single byte to an open local file
 *------------------------------------------------------------------------
 */
devcall	lflPutc (
	  struct dentry *devptr,	/* entry in device switch table */
	  char		ch		/* character (byte) to write	*/
	)
{
	struct	lflcblk	*lfptr;		/* ptr to open file table entry	*/
	struct	ldentry	*ldptr;		/* ptr to file's entry in the	*/
					/*  in-memory directory		*/

	/* Obtain exclusive use of the file */

	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);

	/* If file is not open, return an error */

	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	/* Return SYSERR for an attempt to skip bytes beyond the */
	/* 	current end of the file				 */

	ldptr = lfptr->lfdirptr;
	if (lfptr->lfpos > ldptr->ld_size) {
		signal(lfptr->lfmutex);
		return SYSERR;
	}

	/* If pointer is outside current block, set up new block */

	if (lfptr->lfbyte >= &lfptr->lfdblock[LF_BLKSIZ]) {

		/* Set up block for current file position */
		lfsetup(lfptr);
	}

	/* If appending a byte to the file, increment the file size.	*/
	/* Note: comparison might be equal, but should not be greater.	*/

	if (lfptr->lfpos >= ldptr->ld_size) {
		ldptr->ld_size++;
		Lf_data.lf_dirdirty = TRUE;
	}

	/* Place byte in buffer and mark buffer "dirty" */

	*lfptr->lfbyte++ = ch;
	lfptr->lfpos++;
	lfptr->lfdbdirty = TRUE;

	signal(lfptr->lfmutex);
	return OK;
}
