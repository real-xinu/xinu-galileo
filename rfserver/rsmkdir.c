/* rsmkdir.c - rsmkdir */

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
 * rsmkdir - handle a mkdir request
 *------------------------------------------------------------------------
 */
void	rsmkdir (
	 struct	rf_msg_mreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_mres *resptr	/* ptr to read response		*/
	)
{
	int	retval;			/* return value			*/
/*DEBUG*/ printf("DEBUG: reached rsmkdir\n");

	if (findex >=0) {		/* file exists and is open	*/
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_mres) );
		return;
	}
	retval = mkdir(reqptr->rf_name, S_IRWXU);
	if (retval < 0) {
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_mres) );
			return;
	}

	/* Return OK status */

	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_mres) );
	return;
}
