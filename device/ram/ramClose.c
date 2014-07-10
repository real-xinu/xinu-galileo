/* ramClose.c  -  ramClose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ramClose - Close a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramClose (
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	return OK;
}
