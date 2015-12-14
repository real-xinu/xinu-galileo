/* rstrunc.c - rstrunc */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
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
/* DEBUG */ printf("DEBUG: reached rstrunc\n");

	/* if file is open, close it */

	if (findex >= 0) {
		close(fptr->desc);
		fptr->desc = -1;
	}

	fd = open(reqptr->rf_name, O_TRUNC);
	if (fd < 0) {
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_tres) );
	} else {
		close(fd);
	}

	/* Return OK status */

	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_tres) );
	return;
}
