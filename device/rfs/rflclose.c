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
	int32	retval;			/* Return value			*/
	struct	rf_msg_creq  msg;	/* Request message to send	*/
	struct	rf_msg_cres resp;	/* Buffer for response		*/
	char	*from, *to;		/* Used during name copy	*/
	int32	len;			/* Length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify remote file device is open */

	rfptr = &rfltab[devptr->dvminor];

	/* If device not currently in use, report an error */

	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form read request */

	msg.rf_type = htons(RF_MSG_CREQ);
	msg.rf_status = htons(0);
	msg.rf_seq = 0;			/* Rfscomm will set sequence	*/
	from = rfptr->rfname;
	to = msg.rf_name;
	memset(to, NULLCH, RF_NAMLEN);	/* Start name as all zero bytes	*/
	len = 0;

	while ( (*to++ = *from++) ) {	/* Copy name to request		*/
		if (++len >= RF_NAMLEN) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_creq),
			 (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_cres) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file close\n");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Mark device closed */

	rfptr->rfstate = RF_FREE;
	signal(Rf_data.rf_mutex);
	return OK;
}
