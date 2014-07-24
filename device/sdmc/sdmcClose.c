/* sdmcClose.c  -  sdmcClose */

#include <xinu.h>
#include <sdmc.h>

/*------------------------------------------------------------------------
 * sdmcClose - Close a SD memory card device
 *------------------------------------------------------------------------
 */
devcall	sdmcClose (
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	return OK;
}
