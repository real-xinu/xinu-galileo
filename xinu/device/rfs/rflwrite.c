/* rflwrite.c - rflwrite */

#include <xinu.h>
#if !(RFS_CACHING_ENABLED)
/*------------------------------------------------------------------------
 * rflwrite  -  Write data to a remote file
 *------------------------------------------------------------------------
 */
devcall	rflwrite (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to write	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_wreq  msg;	/* Request message to send	*/
	struct	rf_msg_wres resp;	/* Buffer for response		*/
	char	*from, *to;		/* Used to copy name		*/
	int	i;			/* Counts bytes copied into req	*/
	int32	len;			/* Length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use and mode allows writing */

	rfptr = &rfltab[devptr->dvminor];
	if ( (rfptr->rfstate == RF_FREE) ||
	     ! (rfptr->rfmode & RF_MODE_W) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form write request */

	msg.rf_type = htons(RF_MSG_WREQ);
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
	while ( (*to++ = *from++) ) {	/* Copy name into request	*/
		;
	}
	msg.rf_pos = htonl(rfptr->rfpos);/* Set file position		*/
	msg.rf_len = htonl(count);	/* Set count of bytes to write	*/
	for (i=0; i<count; i++) {	/* Copy data into message	*/
		msg.rf_data[i] = *buff++;
	}
	while (i < RF_DATALEN) {
		msg.rf_data[i++] = NULLCH;
	}

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_wreq),
			 (struct rf_msg_hdr *)&resp,
				sizeof(struct rf_msg_wres) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file read\n");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Report results to caller */

	rfptr->rfpos += ntohl(resp.rf_len);

	signal(Rf_data.rf_mutex);
	return ntohl(resp.rf_len);
}
#else
/* cached implementation */
devcall	rflwrite (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to write	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_wreq  msg;	/* Request message to send	*/
	struct	rf_msg_wres resp;	/* Buffer for response		*/
	char	*from, *to;		/* Used to copy name		*/
	int	j;			/* Counts bytes copied into req	*/
	int32	len;			/* Length of name		*/

	/* for caching */
	uint32  bytes_written;		/* Bytes retrieved */
	uint32  positions[3];			/* positions of (potentially multiple) requests */
	uint32  counts[3];				/* counts of (potentially multiple) requests */

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use and mode allows writing */

	rfptr = &rfltab[devptr->dvminor];
	if ( (rfptr->rfstate == RF_FREE) ||
	     ! (rfptr->rfmode & RF_MODE_W) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* if file size is 0, then that means it may not be initialized, so we'll make a file size request */
	if (rfptr->rfsize == 0) {
		/* signal mutex so rfscontrol() can acquire it */
		signal(Rf_data.rf_mutex);

		/* call rfscontrol() */
		rfscontrol(devptr, RFS_CTL_SIZE, 0, 0);

		/* re-acquire mutex */
		wait(Rf_data.rf_mutex);
	}

	if (rfptr->rfpos > rfptr->rfsize) {
		/* file pointer is beyond EOF */
		#if RFS_CACHE_DEBUG
		kprintf("Trying to write beyond EOF!\n");
		#endif
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* if necessary, split request into multiple requests that each span only one cache block */
	/* if request requires 2 cache blocks, split into two requests */
	if ((rfptr->rfpos + count) / RF_DATALEN > rfptr->rfpos / RF_DATALEN) {
		/* request 1 will be from [rfpos, ((rfpos / RFDATALEN)+1)*RFDATALEN-1]) */
		positions[1] = rfptr->rfpos;
		counts[1] = (rfptr->rfpos / RF_DATALEN)*RF_DATALEN + RF_DATALEN - rfptr->rfpos;
		// counts[1] = (rfptr->rfpos & (~(RF_DATALEN-1))) + RF_DATALEN-1 - rfptr->rfpos;

		/* request 2 will be for [(rfptr->rfpos / RF_DATALEN)*RF_DATALEN + RF_DATALEN, (rfptr->rfpos / RF_DATALEN)*RF_DATALEN + RF_DATALEN + count - counts[0]] */
		positions[2] = (rfptr->rfpos / RF_DATALEN)*RF_DATALEN + RF_DATALEN;
		// positions[2] = (rfptr->rfpos & (~(RF_DATALEN-1))) + RF_DATALEN;
		counts[2] = count - counts[1];
	}
	else {
		/* request 1 (of 1) will be from [rfpos, rfpos + count] */
		positions[1] = rfptr->rfpos;
		counts[1] = count;

		positions[2] = NULL;
		counts[2] = NULL;
	}

	positions[0] = 0;
	counts[0] = 0;
	bytes_written = 0;

	/* loop through sub-requests */
	for (int i = 1; i < 3; i++) {
		/* break once we reach an empty request */
		if (counts[i] == 0) {
			break;
		}

		#if RFS_CACHE_DEBUG
		kprintf("RFL_WRITE: Request %d has file pos %d, count %d\n", i, positions[i], counts[i]);
		#endif

		/* Form write request */
		msg.rf_type = htons(RF_MSG_WREQ);
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
		while ( (*to++ = *from++) ) {	/* Copy name into request	*/
			;
		}

		/* set file position to be start of cache block */
		msg.rf_pos = htonl(positions[i]);/* Set file position		*/
		msg.rf_len = htonl(counts[i]);	/* Set count of bytes to write	*/

		for (j=0; j<counts[i]; j++) {	/* Copy data into message	*/
			msg.rf_data[j] = buff[counts[i-1]+j];
		}
		while (j < RF_DATALEN) {
			msg.rf_data[j++] = NULLCH;
		}

		// #if RFS_CACHE_DEBUG
		// kprintf("data contains %s\n", msg.rf_data);
		// #endif

		/* Send message and receive response */

		retval = rfscomm((struct rf_msg_hdr *)&msg,
						sizeof(struct rf_msg_wreq),
				(struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_wres) );

		/* Check response */

		if (retval == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else if (retval == TIMEOUT) {
			kprintf("Timeout during remote file read\n");
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else if (ntohs(resp.rf_status) != 0) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}

		/* update file size */
		rfptr->rfsize = ntohl(resp.rf_size);
		#if RFS_CACHE_DEBUG
		kprintf("New file size is %d\n", rfptr->rfsize);
		kprintf("Bytes returned: %d\n", ntohl(resp.bytes_returned));
		#endif
		

		/* Report results to caller */

		rfptr->rfpos += ntohl(resp.rf_len);
		bytes_written += ntohl(resp.rf_len);

		/* TODO: If the response is correctly received, implement & call function */
		/* to store the new data into the cache, either through overwriting current block */
		/* or allocating new block if this block isn't in cache yet */
		/* (Server will be modified to always return entire block of data in write response) */ 
		
		/* update file pointer to be at start of block */
		resp.rf_pos = htonl((ntohl(resp.rf_pos) / RF_DATALEN) * RF_DATALEN);

		/* update rf_len to hold number of bytes in data field instead of bytes written */
		resp.rf_len = resp.bytes_returned;

		/* set msg type to write response */
		resp.rf_type = RF_MSG_WRES;

		/* store data returned in cache */
		uint32 status = rfs_cache_store(rfptr, (struct rfs_rres *) &resp);
		if (status == SYSERR) {
			kprintf("RFL_WRITE: Storing in cache failed!\n");
		}
	}

	signal(Rf_data.rf_mutex);
	return bytes_written;
}
#endif