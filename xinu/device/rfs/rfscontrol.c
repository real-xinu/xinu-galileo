/* rfscontrol.c - rfscontrol */

#include <xinu.h>

#define isRFILE(N) ((N >= RFILE0 && N <= RFILE9) ? 1 : 0)

/* Original implementation */
// /*------------------------------------------------------------------------
//  * rfscontrol  -  Provide control functions for the remote file system
//  *------------------------------------------------------------------------
//  */
// devcall	rfscontrol (
// 	 struct dentry	*devptr,	/* Entry in device switch table	*/
// 	 int32	func,			/* A control function		*/
// 	 int32	arg1,			/* Argument #1			*/
// 	 int32	arg2			/* Argument #2			*/
// 	)
// {
// 	int32	len;			/* Length of name		*/
// 	struct	rf_msg_sreq msg;	/* Buffer for size request	*/
// 	struct	rf_msg_sres resp;	/* Buffer for size response	*/
// 	char	*to, *from;		/* Used during name copy	*/
// 	int32	retval;			/* Return value			*/

// 	/* Wait for exclusive access */

// 	wait(Rf_data.rf_mutex);

// 	/* Check length of name (copy during the check even though the	*/
// 	/*	copy is only used for a size request)			*/

// 	from = (char *)arg1;
// 	to = msg.rf_name;
// 	len = 0;
// 	memset(to, NULLCH, RF_NAMLEN);	/* Start name as all zeroes	*/
// 	while ( (*to++ = *from++) ) {	/* Copy name to message		*/
// 		len++;
// 		if (len >= (RF_NAMLEN - 1) ) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		}
// 	}

// 	switch (func) {

// 	/* Delete a file */

// 	case RFS_CTL_DEL:
// 		if (rfsndmsg(RF_MSG_DREQ, (char *)arg1) == SYSERR) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		}
// 		break;

// 	/* Truncate a file */

// 	case RFS_CTL_TRUNC:
// 		if (rfsndmsg(RF_MSG_TREQ, (char *)arg1) == SYSERR) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		}
// 		break;



// 	/* Make a directory */

// 	case RFS_CTL_MKDIR:
// 		if (rfsndmsg(RF_MSG_MREQ, (char *)arg1) == SYSERR) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		}
// 		break;

// 	/* Remove a directory */

// 	case RFS_CTL_RMDIR:
// 		if (rfsndmsg(RF_MSG_XREQ, (char *)arg1) == SYSERR) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		}
// 		break;

// 	/* Obtain current file size (cannot use rfsndmsg because a	*/
// 	/*	response contains a value in addition to the header)	*/

// 	case RFS_CTL_SIZE:

// 		/* Hand-craft a size request message */

// 		msg.rf_type = htons(RF_MSG_SREQ);
// 		msg.rf_status = htons(0);
// 		msg.rf_seq = 0;		/* Rfscomm will set the seq num	*/

// 		/* Send the request to server and obtain a response	*/

// 		retval = rfscomm( (struct rf_msg_hdr *)&msg,
// 					sizeof(struct rf_msg_sreq),
// 				  (struct rf_msg_hdr *)&resp,
// 					sizeof(struct rf_msg_sres) );
// 		if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
// 			signal(Rf_data.rf_mutex);
// 			return SYSERR;
// 		} else {
// 			signal(Rf_data.rf_mutex);
// 			return ntohl(resp.rf_size);
// 		}

// 	default:
// 		kprintf("rfscontrol: function %d not valid\n", func);
// 		signal(Rf_data.rf_mutex);
// 		return SYSERR;
// 	}

// 	signal(Rf_data.rf_mutex);
// 	return OK;
// }

/*------------------------------------------------------------------------
 * rfscontrol  -  Provide control functions for the remote file system
 *------------------------------------------------------------------------
 */
devcall	rfscontrol (
	 struct dentry	*devptr,	/* Entry in device switch table	*/
	 int32	func,			/* A control function		*/
	 int32	arg1,			/* Argument #1			*/
	 int32	arg2			/* Argument #2			*/
	)
{
	int32	len;			/* Length of name		*/
	struct	rf_msg_sreq sreq;	/* Buffer for size request	*/
	struct	rf_msg_sres sres;	/* Buffer for size response	*/
	struct	rf_msg_treq treq;	/* Buffer for truncation request	*/
	struct	rf_msg_tres tres;	/* Buffer for truncation response		*/
	struct	rflcblk	*rfptr;		/* Pointer to entry in rfltab	*/
	char	*to, *from;		/* Used during name copy	*/
	int32	retval;			/* Return value			*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* control function only available to rfl device if func is RFS_CTL_SIZE or RFS_CTL_TRUNC */
	if ( (isRFILE(devptr->dvnum) && ( (func != RFS_CTL_SIZE && func != RFS_CTL_TRUNC) || (struct rflcblk *) (&rfltab[devptr->dvnum-RFILE0])->rfstate == RF_FREE)) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}
	else if ( devptr->dvnum == RFILESYS && (func == RFS_CTL_SIZE || func == RFS_CTL_TRUNC) ) { /* if file size request is from RFILESYS, populate name field from arg 1 */
		/* set request file name to file name passed in arg1 */
		from = (char *) arg1;

		if (func == RFS_CTL_SIZE) {
			to = sreq.rf_name;
		}
		else {
			to = treq.rf_name;
		}
		
		memset(to, NULLCH, RF_NAMLEN);
		len = 0;
		while ( (*to++ = *from++) ) {
			len++;
			if (len >= (RF_NAMLEN - 1) ) {
				signal(Rf_data.rf_mutex);
				return SYSERR;
			}
		}
	}
	else { /* populate name normally */
		/* Check length and copy (needed for size) */
		rfptr = &rfltab[devptr->dvminor];
		from = rfptr->rfname;

		if (func == RFS_CTL_SIZE) {
			to = sreq.rf_name;
		}
		else {
			to = treq.rf_name;
		}

		len = 0;
		memset(to, NULLCH, RF_NAMLEN);	/* Start name as all zeroes	*/
		while ( (*to++ = *from++) ) {	/* Copy name to message		*/
			len++;
			if (len >= (RF_NAMLEN - 1) ) {
				signal(Rf_data.rf_mutex);
				return SYSERR;
			}
		}
	}

	

	switch (func) {

	/* Delete a file */

	case RFS_CTL_DEL:
		if (rfsndmsg(RF_MSG_DREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Truncate a file */

	case RFS_CTL_TRUNC:
		/* Original implementation */
		// if (rfsndmsg(RF_MSG_TREQ, (char *)arg1) == SYSERR) {
		// 	signal(Rf_data.rf_mutex);
		// 	return SYSERR;
		// }

		/* New implementation that allows user to specify truncation size */
		if ( (isRFILE(devptr->dvnum) && arg1 < 0) || (!isRFILE(devptr->dvnum) && arg2 < 0) ) {
			/* can't truncate to length below zero */
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}

		/* Form a request */
		/* set truncation size */
		if (isRFILE(devptr->dvnum)) {
			treq.trunc_size = htons(arg1);
		}
		else {
			treq.trunc_size = htons(arg2);
		}
	
		treq.rf_type = htons(RF_MSG_TREQ);
		treq.rf_status = htons(0);
		treq.rf_seq = 0;			/* Rfscomm will set sequence	*/

		/* Send message and receive response */

		retval = rfscomm(&treq,  sizeof(struct rf_msg_treq),
				&tres,	sizeof(struct rf_msg_hdr) );

		/* Check response */

		if (retval == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else if (retval == TIMEOUT) {
			kprintf("Timeout during remote file server access\n");
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else if (ntohl(tres.rf_status) != 0) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}

		/* if called from a rfl device, update it's local file size */
		#if RFS_CACHING_ENABLED
		if (isRFILE(devptr->dvnum)) {
			(&rfltab[devptr->dvnum-RFILE0])->rfsize = arg1;
		}
		#endif
		break;

	/* Make a directory */

	case RFS_CTL_MKDIR:
		if (rfsndmsg(RF_MSG_MREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Remove a directory */

	case RFS_CTL_RMDIR:
		if (rfsndmsg(RF_MSG_XREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Obtain current file size (non-standard message size) */

	case RFS_CTL_SIZE:
		/* Hand-craft a size request message */

		sreq.rf_type = htons(RF_MSG_SREQ);
		sreq.rf_status = htons(0);
		sreq.rf_seq = 0;		/* Rfscomm will set the seq num	*/

		/* Send the request to server and obtain a response	*/

		retval = rfscomm( (struct rf_msg_hdr *)&sreq,
					sizeof(struct rf_msg_sreq),
				  (struct rf_msg_hdr *)&sres,
					sizeof(struct rf_msg_sres) );
		if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else {
			/* update file size if called by file device directly */
			#if RFS_CACHING_ENABLED
			if (isRFILE(devptr->dvnum)) {
				(&rfltab[devptr->dvnum-RFILE0])->rfsize = ntohl(sres.rf_size);
				#if RFS_CACHE_DEBUG
				kprintf("Updated file size to be %u bytes\n", (&rfltab[devptr->dvnum-RFILE0])->rfsize);
				#endif
			}
			#endif

			signal(Rf_data.rf_mutex);
			return ntohl(sres.rf_size);
		}

	default:
		kprintf("rfscontrol: function %d not valid\n", func);
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	signal(Rf_data.rf_mutex);
	return OK;
}
