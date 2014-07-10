/* namOpen.c - namOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  namOpen  -  open a file or device based on the name
 *------------------------------------------------------------------------
 */
devcall	namOpen(
	  struct dentry *devptr,	/* entry in device switch table */
	  char	*name,			/* name to open			*/
	  char	*mode			/* mode argument		*/
	)
{
	char	newname[NM_MAXLEN];	/* name with prefix replaced	*/
	did32	newdev;			/* device ID after mapping	*/

	/* use namespace to map name to a new name and new descriptor */

	newdev = nammap(name, newname, devptr->dvnum);
	
	if (newdev == SYSERR) {
		return SYSERR;
	}

	/* open underlying device and return status */

	return  open(newdev, newname, mode);
}
