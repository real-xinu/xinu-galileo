/* rsclose.c - rsclose */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * rsclose - handle a close request
 *------------------------------------------------------------------------
 */
void	rsclose (
	struct	rf_msg_creq *reqptr,	/* ptr to close request	*/
	struct	rf_msg_cres *resptr	/* ptr to close response*/
	)
{

	if(findex < 0) {
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct	rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_cres) );
		return;
	}

	if(ofiles[findex].desc >= 0) {
		close(ofiles[findex].desc);
		ofiles[findex].desc = -1;
	}
	else if(ofiles[findex].dirptr != NULL) {
		closedir(ofiles[findex].dirptr);
		ofiles[findex].dirptr = NULL;
	}

	memset(ofiles[findex].name, '\0', RF_NAMLEN+1);

	sndok( (struct rf_msg_hdr *)reqptr,
	       (struct rf_msg_hdr *)resptr,
	       sizeof(struct rf_msg_cres) );

	return;
}
