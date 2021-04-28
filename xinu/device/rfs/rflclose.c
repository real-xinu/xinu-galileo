/* rflclose.c - rflclose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflclose  -  Close a remote file device
 *------------------------------------------------------------------------
 */
devcall	rflclose (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify remote file device is open */

	rfptr = &rfltab[devptr->dvminor];
	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Mark device closed */

	/* TODO: add any needed code to clear up cache prior to next rfl allocation */

	#if RFS_CACHING_ENABLED
	/* reset file size to 0 */
	rfptr->rfsize = 0;

	/* We can either de-allocate all the cache blocks now, or just set their valid_bytes to 0 and de-alloc as needed later */
	/* set all cache blocks to have valid_bytes be 0 */

	/* iterate over array, set any non-NULL blocks to have 0 valid_bytes */
	for (int i = 0; i < MAX_RFS_CBLOCKS; i++) {
		struct rfs_cblock *block = rfptr->cache[i];
		if (block != NULL) {
			block->valid_bytes = 0;
		}
	}

	/* iterate over list, set all blocks to have 0 valid_bytes */
	struct rfs_cblock *cur = rfptr->cache_list;
	while (cur != NULL) {
		cur->valid_bytes = 0;
		cur = cur->next;
	}
	#endif

	/* set state to free */
	rfptr->rfstate = RF_FREE;
	signal(Rf_data.rf_mutex);
	return OK;
}
