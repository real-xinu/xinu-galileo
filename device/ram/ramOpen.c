/* ramOpen.c  -  ramOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ramOpen - open a ram disk
 *------------------------------------------------------------------------
 */

devcall	ramOpen (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 char	*name,			/* unused for a ram disk	*/
	 char	*mode			/* unused for a ram disk	*/
	)
{
	/* return device descriptor */

	return devptr->dvnum;
}
