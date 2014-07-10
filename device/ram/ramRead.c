/* ramRead.c  -  ramRead */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * ramRead - Read a block from a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramRead (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer to hold disk block	*/
	  int32	blk			/* block number of block to read*/
	)
{
	int32	bpos;			/* byte position of blk		*/

	bpos = RM_BLKSIZ * blk;
	memcpy(buff, &Ram.disk[bpos], RM_BLKSIZ);
	return OK;
}
