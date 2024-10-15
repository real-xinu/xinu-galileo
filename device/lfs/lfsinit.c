/* lfsinit.c - lfsinit */

#include <xinu.h>

struct	lfdata	Lf_data;

/*------------------------------------------------------------------------
 * lfsinit  -  Initialize the local file system primary device
 *------------------------------------------------------------------------
 */
devcall	lfsinit (
	  struct dentry *devptr		/* Entry in device switch table */
	)
{
	/* Indicate that no disk device has been selected */

	Lf_data.lf_dskdev = -1;

	/* Create a mutual exclusion semaphore */

	Lf_data.lf_mutex = semcreate(1);

	/* Zero directory area (for debugging) */

	memset((char *)&Lf_data.lf_dir, NULLCH, sizeof(struct lfdir));

	/* Initialize directory to "not present" in memory */

	Lf_data.lf_dirpresent = Lf_data.lf_dirdirty = FALSE;

	return OK;
}
