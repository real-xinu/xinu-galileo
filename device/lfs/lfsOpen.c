/* lfsOpen.c  -  lfsOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lfsOpen - open a file and allocate a local file pseudo-device
 *------------------------------------------------------------------------
 */
devcall	lfsOpen (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 char	*name,			/* name of file to open		*/
	 char	*mode			/* mode chars: 'r' 'w' 'o' 'n'	*/
	)
{
	struct	lfdir	*dirptr;	/* ptr to in-memory directory	*/
	char		*from, *to;	/* ptrs used during copy	*/
	char		*nam, *cmp;	/* ptrs used during comparison	*/
	int32		i;		/* general loop index		*/
	did32		lfnext;		/* minor number of an unused	*/
					/*    file pseudo-device	*/
	struct	ldentry	*ldptr;		/* ptr to an entry in directory	*/
	struct	lflcblk	*lfptr;		/* ptr to open file table entry	*/
	bool8		found;		/* was the name found?		*/
	int32	retval;			/* value returned from function	*/
	int32	mbits;			/* mode bits			*/

	/* Check length of name file (leaving space for NULLCH */

	from = name;
	for (i=0; i< LF_NAME_LEN; i++) {
		if (*from++ == NULLCH) {
			break;
		}
	}
	if (i >= LF_NAME_LEN) {		/* name is too long */
		return SYSERR;
	}

	/* Parse mode argument and convert to binary */

	mbits = lfgetmode(mode);
	if (mbits == SYSERR) {
		return SYSERR;
	}

	/* If named file is already open, return SYSERR */

	lfnext = SYSERR;
	for (i=0; i<Nlfl; i++) {	/* search file pseudo-devices	*/
		lfptr = &lfltab[i];
		if (lfptr->lfstate == LF_FREE) {
			if (lfnext == SYSERR) {
				lfnext = i; /* record index */
			}
			continue;
		}

		/* Compare requested name to name of open file */

		nam = name;
		cmp = lfptr->lfname;
		while(*nam != NULLCH) {
			if (*nam != *cmp) {
				break;
			}
			nam++;
			cmp++;
		}

		/* See if comparison succeeded */

		if ( (*nam==NULLCH) && (*cmp == NULLCH) ) {
			return SYSERR;
		}
	}
	if (lfnext == SYSERR) {	/* no slave file devices are available	*/
		return SYSERR;
	}

	/* Obtain copy of directory if not already present in memory	*/

	dirptr = &Lf_data.lf_dir;
	wait(Lf_data.lf_mutex);
	if (! Lf_data.lf_dirpresent) {
	    retval = read(Lf_data.lf_dskdev,(char *)dirptr,LF_AREA_DIR);
	    if (retval == SYSERR ) {
		signal(Lf_data.lf_mutex);
		return SYSERR;
	    }
	    Lf_data.lf_dirpresent = TRUE;
	}

	/* Search directory to see if file exists */

	found = FALSE;
	for (i=0; i<dirptr->lfd_nfiles; i++) {
		ldptr = &dirptr->lfd_files[i];
		nam = name;
		cmp = ldptr->ld_name;
		while(*nam != NULLCH) {
			if (*nam != *cmp) {
				break;
			}
			nam++;
			cmp++;
		}
		if ( (*nam==NULLCH) && (*cmp==NULLCH) ) { /* name found	*/
			found = TRUE;
			break;
		}
	}

	/* Case #1 - file is not in directory (i.e., does not exist)	*/

	if (! found) {
		if (mbits & LF_MODE_O) {	/* file *must* exist	*/
			signal(Lf_data.lf_mutex);
			return SYSERR;
		}

		/* Take steps to create new file and add to directory	*/

		/* Verify that space remains in the directory */

		if (dirptr->lfd_nfiles >= LF_NUM_DIR_ENT) {
			signal(Lf_data.lf_mutex);
			return SYSERR;
		}

		/* Allocate next dir. entry & initialize to empty file	*/

		ldptr = &dirptr->lfd_files[dirptr->lfd_nfiles++];
		ldptr->ld_size = 0;
		from = name;
		to = ldptr->ld_name;
		while ( (*to++ = *from++) != NULLCH ) {
			;
		}
		ldptr->ld_ilist = LF_INULL;

	/* Case #2 - file is in directory (i.e., already exists)	*/

	} else if (mbits & LF_MODE_N) {		/* file must not exist	*/
			signal(Lf_data.lf_mutex);
			return SYSERR;
	}

	/* Initialize the local file pseudo-device */

	lfptr = &lfltab[lfnext];
	lfptr->lfstate = LF_USED;
	lfptr->lfdirptr = ldptr;	/* point to directory entry	*/
	lfptr->lfmode = mbits & LF_MODE_RW;

	/* File starts at position 0 */

	lfptr->lfpos     = 0;

	to = lfptr->lfname;
	from = name;
	while ( (*to = *from++) != NULLCH ) {
		;
	}

	/* Neither index block nor data block are initially valid	*/

	lfptr->lfinum    = LF_INULL;
	lfptr->lfdnum    = LF_DNULL;

	/* Initialize byte pointer to address beyond the end of the	*/
	/*	buffer (i.e., invalid pointer triggers setup)		*/

	lfptr->lfbyte = &lfptr->lfdblock[LF_BLKSIZ];
	lfptr->lfibdirty = FALSE;
	lfptr->lfdbdirty = FALSE;

	signal(Lf_data.lf_mutex);

	return lfptr->lfdev;
}
