/* rsdelete.c - rsdelete */

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
 * rsdelete - handle a delete request
 *------------------------------------------------------------------------
 */
void	rsdelete (
	 struct	rf_msg_dreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_dres *resptr	/* ptr to read response		*/
	)
{
	int	retval;			/* return value			*/
/*DEBUG*/ printf("DEBUG: reached rsdelete\n");

	/* if file is open, close it */

	if (findex >= 0) {
		close(fptr->desc);
		fptr->desc = -1;
	}

	retval = unlink(reqptr->rf_name);
	if (retval < 0) {
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_dres) );
			return;
	}

	/* Return OK status */

	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_sres) );
	return;
}
