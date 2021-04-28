/* rflread.c - rflread */

#include <xinu.h>

#if !(RFS_CACHING_ENABLED)
/*------------------------------------------------------------------------
 * rflread  -  Read data from a remote file
 *------------------------------------------------------------------------
 */
devcall	rflread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to read	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_rreq  msg;	/* Request message to send	*/
	struct	rf_msg_rres resp;	/* Buffer for response		*/
	int32	i;			/* Counts bytes copied		*/
	char	*from, *to;		/* Used during name copy	*/
	int32	len;			/* Length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use */

	rfptr = &rfltab[devptr->dvminor];

	/* If device not currently in use, report an error */

	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device allows reading */

	if ((rfptr->rfmode & RF_MODE_R) == 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form read request */

	msg.rf_type = htons(RF_MSG_RREQ);
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
	msg.rf_pos = htonl(rfptr->rfpos);/* Set file position		*/
	msg.rf_len = htonl(count);	/* Set count of bytes to read	*/

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_rreq),
			 (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_rres) );

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

	/* Determine number of bytes to copy into buffer */
	uint32 copy_length;
	if (ntohl(resp.rf_len) > count) {
		copy_length = count;
	}
	else {
		copy_length = ntohl(resp.rf_len);
	}

	/* 
	 * Instead of always copying reply length,
	 * copy as much data without overflowing the buffer.
	 */
	// for (i=0; i<ntohl(resp.rf_len); i++) {
	// 	*buff++ = resp.rf_data[i];
	// }
	// rfptr->rfpos += ntohl(resp.rf_len);
	for (i=0; i<copy_length; i++) {
		*buff++ = resp.rf_data[i];
	}
	rfptr->rfpos += copy_length;
	

	signal(Rf_data.rf_mutex);
	//return ntohl(resp.rf_len);
	return copy_length;
}
#else
/* updated implementation that utilizes caching */
devcall	rflread (
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of bytes		*/
	  int32	count 			/* Count of bytes to read	*/
	)
{
	struct	rflcblk	*rfptr;		/* Pointer to control block	*/
	int32	retval;			/* Return value			*/
	struct	rf_msg_rreq  msg;	/* Request message to send	*/
	struct	rf_msg_rres resp;	/* Buffer for response		*/
	int32	i;			/* Counts bytes copied		*/
	char	*from, *to;		/* Used during name copy	*/
	int32	len;			/* Length of name		*/

	/* for caching */
	uint32  bytes_retrieved;		/* Bytes retrieved */
	uint32  positions[3];			/* positions of (potentially multiple) requests */
	uint32  counts[3];				/* counts of (potentially multiple) requests */

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use */

	rfptr = &rfltab[devptr->dvminor];

	/* If device not currently in use, report an error */

	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device allows reading */

	if ((rfptr->rfmode & RF_MODE_R) == 0) {
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

	/* if file size is still 0 after making this request, then no data to read */
	if (rfptr->rfsize == 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* truncate request so it does not go beyond current file size */
	if (rfptr->rfpos + count > rfptr->rfsize) {
		/* reduce count to be rfsize - rfpos bytes */
		count = rfptr->rfsize - rfptr->rfpos;
	}

	/* re-check that count is valid */
	if ( (count <= 0) || (count > RF_DATALEN) ) {
		#if RFS_CACHE_DEBUG
		kprintf("RFL_READ: Trying to read beyond file length!\n");
		#endif
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

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

		positions[2] = 0;
		counts[2] = 0;
	}

	#if RFS_CACHE_DEBUG
	kprintf("RFL_READ: Request 1 has file pos %d, count %d\n", positions[1], counts[1]);
	kprintf("RFL_READ: Request 2 has file pos %d, count %d\n", positions[2], counts[2]);
	#endif

	positions[0] = 0;
	counts[0] = 0;
	bytes_retrieved = 0;
	/* loop through all valid requests */
	for (i = 1; i < 3; i++) {
		/* break once we reach an empty request */
		if (counts[i] == 0) {
			break;
		}

		/* check if desired data exists in cache; place in buff if so */
		bytes_retrieved = rfs_cache_fetch(rfptr, positions[i], buff + counts[i-1], counts[i]);

		if (bytes_retrieved == counts[i]) {
			/* got desired data */
			continue;
		}

		#if RFS_CACHE_DEBUG
		kprintf("RFLREAD: Data for request %d not cached, going to make a request...\n", i);
		#endif

		/* Data not found in cache, form read request */
		msg.rf_type = htons(RF_MSG_RREQ);
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

		/* request will be for a full cache block */
		msg.rf_pos = htonl((positions[i] / RF_DATALEN) * RF_DATALEN); /* Set file position		*/
		msg.rf_len = htonl(RF_DATALEN);	/* Set count of bytes to read	*/

		#if RFS_CACHE_DEBUG
		kprintf("RFL_READ: Making network request for %d bytes from pos %d\n", RF_DATALEN, (positions[i] / RF_DATALEN) * RF_DATALEN);
		#endif

		/* Send message and receive response */

		retval = rfscomm((struct rf_msg_hdr *)&msg,
						sizeof(struct rf_msg_rreq),
				(struct rf_msg_hdr *)&resp,
						sizeof(struct rf_msg_rres) );

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

		/* Update file size */
		#if RFS_CACHE_DEBUG
		kprintf("The returned file size was %u bytes!\n", ntohl(resp.rf_size));
		#endif
		rfptr->rfsize = ntohl(resp.rf_size);

		/* TODO: Cache data received from request and re-fetch from cache */
		
		/* Code for copying data to cache goes here */
		retval = rfs_cache_store(rfptr, &resp);
		if (retval == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}

		/* Re-fetch data from cache */
		bytes_retrieved = rfs_cache_fetch(rfptr, positions[i], buff + counts[i-1], counts[i]);

		if (bytes_retrieved != counts[i]) {
			/* can't reach data even after making request to data, there is a problem */
			#if RFS_CACHE_DEBUG
			kprintf("RFL_READ: Request %d - only found %d bytes - can't reach data even after making request to data, there is a problem\n", i, bytes_retrieved);
			#endif
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}	

	signal(Rf_data.rf_mutex);
	//return ntohl(resp.rf_len);
	return count;
}
#endif

// /* wrapper function that is mapped to read in the device switch table */
// devcall	rflread_wrapper (
// 	  struct dentry	*devptr,	/* Entry in device switch table	*/
// 	  char	*buff,			/* Buffer of bytes		*/
// 	  int32	count 			/* Count of bytes to read	*/
// 	)
// {
// 	#if RFS_CACHING_ENABLED
// 	return rflread_cached(devptr, buff, count);
// 	#else
// 	return rflread(devptr, buff, count);
// 	#endif
// }