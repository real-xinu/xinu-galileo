/* rdclose.c - rdclose */

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
 * rdclose - handle a close request
 *------------------------------------------------------------------------
 */
void	rdclose (
	 struct	rd_msg_creq *reqptr,	/* ptr to read request		*/
	 struct	rd_msg_cres *resptr	/* ptr to read response		*/
	)
{
	int	retval;			/* return value			*/

#ifdef DEBUG
	printf("DEBUG: reached rdclose\n");
#endif

	if (findex < 0) {		/* disk is not open		*/
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			 sizeof(struct rd_msg_cres) );
		return;
	}
	close(ofiles[findex].desc);
	ofiles[findex].desc = -1;

	/* Return OK status */

	sndok ( (struct rd_msg_hdr *)reqptr,
		(struct rd_msg_hdr *)resptr,
		 sizeof(struct rd_msg_cres) );
	return;
}
