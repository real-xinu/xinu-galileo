/* sdmcWrite.c  -  sdmcWrite */

#include <xinu.h>
#include <sdmc.h>

/*------------------------------------------------------------------------
 * sdmcWrite - Write a buffer to an SD memory card
 *------------------------------------------------------------------------
 */
devcall	sdmcWrite (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer to hold disk block	*/
	  int32	blk			/* block number of disk block	*/
	)
{


	return OK;
}
