/* sdmcRead.c  -  sdmcRead */

#include <xinu.h>
#include <sdmc.h>

/*------------------------------------------------------------------------
 * ramRead - Read data from an SD memory card
 *------------------------------------------------------------------------
 */
devcall	sdmcRead (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer to hold disk block	*/
	  int32	blk			/* block number of block to read*/
	)
{


	return OK;
}
