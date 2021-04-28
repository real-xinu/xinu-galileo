/* rsread.c - rsread */

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

extern	void*	memset(void *, int, size_t);

/*------------------------------------------------------------------------
 * rsread - handle a read request
 *------------------------------------------------------------------------
 */
void	rsread (
	 struct	rf_msg_rreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_rres *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	int	i, j;			/* general loop indexes		*/
	struct	stat	buf;		/* buffer for file status	*/
	int	retval;			/* function return value	*/
	char	*to, *from;		/* used during copy		*/
	int	nbytes;			/* num. of bytes read from file	*/

	retval = stat(reqptr->rf_name, &buf);
	if(retval >= 0) {
		if(buf.st_mode & S_IFDIR) {
			rsdirread(reqptr, resptr);
			return;
		}
	}

	/* update file size */
	printf("File size is %d bytes\n", buf.st_size);
	resptr->rf_size = htonl(buf.st_size);

	/* if file is not open, try to open it */

	if (findex < 0) {
		if ( (fd = rsofile(reqptr->rf_name,O_RDWR)) < 0 ) {
			resptr->rf_pos = reqptr->rf_pos;
			resptr->rf_len = 0; /* set length to zero */
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				sizeof(struct rf_msg_rreq) );
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
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_rreq) );
		return;
	}

	/* seek to specified offset and read data */

	lseek(fd, ntohl(reqptr->rf_pos), SEEK_SET);
	memset(resptr->rf_data, NULLCH, RF_DATALEN);
	nbytes = read(fd, resptr->rf_data, ntohl(reqptr->rf_len));

#ifdef DEBUG
	printf("DEBUG: read from desc %d specified file returns %d \n",
			fd, nbytes);
#endif
	
	if (nbytes < 0) {
		resptr->rf_pos = reqptr->rf_pos;
		resptr->rf_len = 0; /* set length to zero */
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_rreq) );
		return;
	}

	/* set bytes read */
	
	resptr->rf_pos = reqptr->rf_pos;
	resptr->rf_len = htonl(nbytes);
	sndok(	(struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		sizeof(struct rf_msg_rres) );
	return;
}
