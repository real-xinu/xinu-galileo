/* rfsinit.c - rfsinit */

#include <xinu.h>

struct	rfdata	Rf_data;

/*------------------------------------------------------------------------
 *  rfsinit  -  Initialize the remote file system primary device
 *------------------------------------------------------------------------
 */
devcall	rfsinit(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{


	/* Set an initial message sequence number */

	Rf_data.rf_seq = 1;

	/* Set the server IP address to zero until rfscomm is called */

	Rf_data.rf_ser_ip = 0;

	/* Record the local and remote port numbers to use */

	Rf_data.rf_ser_port = RF_SERVER_PORT;
	Rf_data.rf_loc_port = RF_LOC_PORT;

	/* Create a mutual exclusion semaphore */

	if ( (Rf_data.rf_mutex = semcreate(1)) == SYSERR ) {
		panic("Cannot create remote file system semaphore");
	}

	/* Specify that the UDP slot is not yet registered */

	Rf_data.rf_registered = FALSE;

	return OK;
}
