/* rfsinit.c - rfsinit */

#include <xinu.h>

struct	rfdata	Rf_data;

/*------------------------------------------------------------------------
 *  rfsinit  -  Initialize the remote file system master device
 *------------------------------------------------------------------------
 */
devcall	rfsinit(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{

	/* Choose an initial message sequence number */

	Rf_data.rf_seq = 1;

	/* Set the server IP address, server port, and local port */

	if ( dot2ip(RF_SERVER_IP, &Rf_data.rf_ser_ip) == SYSERR ) {
		panic("invalid IP address for remote file server");
	}
	Rf_data.rf_ser_port = RF_SERVER_PORT;
	Rf_data.rf_loc_port = RF_LOC_PORT;

	/* Create a mutual exclusion semaphore */

	if ( (Rf_data.rf_mutex = semcreate(1)) == SYSERR ) {
		panic("Cannot create remote file system semaphore");
	}

	/* Specify that the server port is not yet registered */

	Rf_data.rf_registered = FALSE;

	/* the following initialization needs to be done only if caching is enabled */
	#if RFS_CACHING_ENABLED
	/* set-up for LRU list of cached blocks */
	Rf_data.lru_head = NULL;
	Rf_data.lru_tail = NULL;
	Rf_data.num_cblocks = 0;

	/* initialize buffer pool for allocable cache blocks */
	Rf_data.buffpoolid = mkbufpool(sizeof(struct rfs_cblock), MAX_CBLOCKS_ALLOCABLE);
	if (Rf_data.buffpoolid == SYSERR) {
		panic("Cannot allocate buffer pool");
	}
	#endif

	return OK;
}
