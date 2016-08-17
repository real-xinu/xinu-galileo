/* rddelete.c - rddelete */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rdisksys.h"
#include "rdserver.h"

/*------------------------------------------------------------------------
 * rddelete - handle a delete request
 *------------------------------------------------------------------------
 */
void	rddelete (
	 struct	rd_msg_dreq *reqptr,	/* ptr to read request		*/
	 struct	rd_msg_dres *resptr	/* ptr to read response		*/
	)
{
	int	retval;			/* return value			*/

#ifdef DEBUG
	printf("DEBUG: reached rddelete\n");
#endif

	/* if disk is open, close it */

	if (findex >= 0) {
		close(fptr->desc);
		fptr->desc = -1;
	}

	retval = unlink(reqptr->rd_id);
	if (retval < 0) {
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			 sizeof(struct rd_msg_dres) );
			return;
	}

	/* Return OK status */

	sndok ( (struct rd_msg_hdr *)reqptr,
		(struct rd_msg_hdr *)resptr,
		 sizeof(struct rd_msg_dres) );
	return;
}
