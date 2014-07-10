/* ramInit.c  -  ramInit */

#include <xinu.h>
#include <ramdisk.h>

struct	ramdisk	Ram;

/*------------------------------------------------------------------------
 *  ramInit - initialize the remote disk system device
 *------------------------------------------------------------------------
 */
devcall	ramInit (
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	memcpy(Ram.disk, "hopeless", 8);
	memcpy( &Ram.disk[8], Ram.disk, RM_BLKSIZ * RM_BLKS - 8);
	return OK;
}
