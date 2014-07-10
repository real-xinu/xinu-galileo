/* ramWrite.c  -  ramWrite */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * ramWrite - Write a block to a ram disk
 *------------------------------------------------------------------------
 */
devcall	ramWrite (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer to hold disk block	*/
	  int32	blk			/* block number of disk block	*/
	)
{
	int32	bpos;			/* byte position of blk		*/

	bpos = RM_BLKSIZ * blk;
	memcpy(&Ram.disk[bpos], buff, RM_BLKSIZ);
	return OK;
}
