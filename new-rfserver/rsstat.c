/* rsstat.c - rsstat */

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
 * rsstat - handle a stat request
 *------------------------------------------------------------------------
 */
void	rsstat (
	 struct	rf_msg_sreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_sres *resptr	/* ptr to read response		*/
	)
{
	struct	stat	stbuf;		/* buffer for file status	*/
	int	sreturn;		/* stat return value		*/
#ifdef DEBUG
printf("DEBUG: reached rsstat\n");
#endif

	printf("Requested file size of %s\n", reqptr->rf_name);
	sreturn = stat(reqptr->rf_name, &stbuf);

	if (sreturn < 0) {	/* file does not exist */
		resptr->rf_size = 0;
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_sres) );
		return;
	}

	/* Place the file size in the response message */

	resptr->rf_size = htonl(stbuf.st_size);

	/* Copy the header and send the response */

	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_sres) );
	return;
}
