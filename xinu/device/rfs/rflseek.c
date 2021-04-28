/* rflseek.c - rflseek */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflseek  -  Change the current position in an open file
 *------------------------------------------------------------------------
 */
devcall	rflseek (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  uint32 pos			/* New file position		*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify remote file device is open */

	rfptr = &rfltab[devptr->dvminor];

	#if RFS_CACHING_ENABLED
	/* if file size is 0, then that means it may not be initialized, so we'll make a file size request */
	if (rfptr->rfsize == 0) {
		/* signal mutex so rfscontrol() can acquire it */
		signal(Rf_data.rf_mutex);

		/* call rfscontrol() */
		rfscontrol(devptr, RFS_CTL_SIZE, 0, 0);

		/* re-acquire mutex */
		wait(Rf_data.rf_mutex);
	}
	
	/* if caching is enabled, then we track file size locally, so can check seek goes beyond EOF */
	if (pos > rfptr->rfsize) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}
	#endif

	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Set the new position */

	rfptr->rfpos = pos;
	signal(Rf_data.rf_mutex);
	return OK;
}
