/* rswrite.c - rswrite */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * rswrite - handle a write request
 *------------------------------------------------------------------------
 */
void	rswrite (
	 struct	rf_msg_wreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_wres *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	struct	stat	buf;		/* buffer for file status	*/
	int	retval;			/* function return value	*/
	int	nbytes;			/* num. of bytes read from file	*/
	int	i;			/* index of ofiles table	*/
	char	*from, *to;		/* used during name copy	*/

	/* if file is not open, try to open it */

	if (findex < 0) {
		if ( (fd = rsofile(reqptr->rf_name,O_RDWR)) < 0 ) {
			resptr->rf_pos = reqptr->rf_pos;
			resptr->rf_len = 0; /* set length to zero */
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				sizeof(struct rf_msg_wres) );
			return;
		}
		for (i=0; i<MAXFILES; i++) {

			/* If entry is free, stop searching */

			if (ofiles[i].desc < 0) {
				break;
			}
		}
		if (i >= MAXFILES) {	/* table is full */
			i = fnext;	/* choose entry to close */
			close(ofiles[fnext].desc);

			/* move to next slot for the future , wrap */
			/*	around the table, if necessary	   */

			fnext++;
			if (fnext >= MAXFILES) {
				fnext = 0;
			}
		}
		ofiles[i].desc = fd;
		from = reqptr->rf_name;
		to =ofiles[i].name;

		/* copy name to open file table */

		while ( (*to++ = *from++) ) {
			;
		}
	} else {
		fd = fptr->desc;
	}

	/* stat file to get its current size */

	retval = fstat(fd, &buf);

	/* if requested offset is beyond EOF, return error */

	if ( (retval<0) || (ntohl(reqptr->rf_pos) > buf.st_size) ) {
		resptr->rf_pos = reqptr->rf_pos;
		resptr->rf_len = 0;	 /* set length to zero */

#ifdef DEBUG
		printf("DEBUG: requested offset %d is beyond %lu\n",
			reqptr->rf_pos, buf.st_size);
#endif

		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_wres) );
		return;
	}

	/* seek to specified offset and write data */

	lseek(fd, ntohl(reqptr->rf_pos), SEEK_SET);
	nbytes = write(fd, reqptr->rf_data, ntohl(reqptr->rf_len));
	
	if (nbytes < 0) {

#ifdef DEBUG
		printf("DEBUG: write failed\n");
#endif

		resptr->rf_pos = reqptr->rf_pos;
		resptr->rf_len = 0; /* set length to zero */
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_wres) );
		return;
	}

	/** Added for cache implementation **/
	/* save bytes written in response length */
	resptr->rf_len = htonl(nbytes);

	/* set file size */
	retval = fstat(fd, &buf);
	resptr->rf_size = htonl(buf.st_size);

	/* return the cache block corresponding to the written data */
	lseek(fd, (ntohl(reqptr->rf_pos) / RF_DATALEN) * RF_DATALEN, SEEK_SET);
	memset(resptr->rf_data, NULLCH, RF_DATALEN);
	nbytes = read(fd, resptr->rf_data, RF_DATALEN);

	/* save bytes returned */
	resptr->bytes_returned = htonl(nbytes);
	/** **/

	/* set bytes read */
	
	resptr->rf_pos = reqptr->rf_pos;
	// resptr->rf_len = htonl(nbytes);	/* set earlier */
	sndok(	(struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		sizeof(struct rf_msg_wres) );
	return;
}
