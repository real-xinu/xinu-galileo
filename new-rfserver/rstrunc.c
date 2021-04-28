/* rstrunc.c - rstrunc */

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
 * rstrunc - handle a truncate request
 *------------------------------------------------------------------------
 */
void	rstrunc (
	 struct	rf_msg_treq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_tres *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	int status;		/* status of call to truncate() or ftruncate() */
/* DEBUG */ printf("DEBUG: reached rstrunc\n");

	/* if file is open, close it */

	/* Original implementation */
	// if (findex >= 0) {
	// 	close(fptr->desc);
	// 	fptr->desc = -1;
	// }

	// fd = open(reqptr->rf_name, O_TRUNC);
	// if (fd < 0) {
	// 	snderr( (struct rf_msg_hdr *)reqptr,
	// 		(struct rf_msg_hdr *)resptr,
	// 		 sizeof(struct rf_msg_tres) );
	// } else {
	// 	close(fd);
	// }

	/* New implementation that allows user to specify truncation size */
	if (ntohs(reqptr->trunc_size) < 0) {
		/* cannot truncate to less than 0 bytes */
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_tres) );
		return;
	}

	if (findex >= 0) {
		/* file is already open, use ftruncate */
		status = ftruncate(fptr->desc, ntohs(reqptr->trunc_size));
		if (status != 0) {
			/* error with ftruncate */
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				sizeof(struct rf_msg_tres) );
			return;
		}
	}
	else {
		/* file is not already open, use truncate */
		status = truncate(reqptr->rf_name, ntohs(reqptr->trunc_size));
		if (status != 0) {
			/* error with ftruncate */
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				sizeof(struct rf_msg_tres) );
			return;
		}
	}
	
	/* Return OK status */

	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_tres) );
	return;
}
