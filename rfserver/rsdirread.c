/* rsdirread.c - rsdirread */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * rsdirread - handle a directory read request
 *------------------------------------------------------------------------
 */
void	rsdirread (
	struct	rf_msg_rreq *reqptr,	/* Read request pointer	*/
	struct	rf_msg_rres *resptr	/* Read response pointer*/
	)
{
	struct	dirent *dentry;	/* An entry in the directory	*/
	struct	rfdirent *rfdentry;
	char	*to, *from;	/* used to copy names		*/

	if(findex < 0) {
		snderr( (struct rf_msg_hdr *)reqptr,
			 (struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_rreq) );
		return;
	}

	memset(resptr->rf_data, NULLCH, RF_DATALEN);
	dentry = readdir(ofiles[findex].dirptr);
	if(dentry != NULL) {
		rfdentry = (struct rfdirent *)resptr->rf_data;
		if(dentry->d_type == DT_DIR) {
			rfdentry->d_type = RF_DIRENT_DIR;
		}
		else {
			rfdentry->d_type = RF_DIRENT_FILE;
		}
		to = rfdentry->d_name;
		from = dentry->d_name;
		while(*to++ = *from++) {
			;
		}
		resptr->rf_len = htonl(sizeof(struct rfdirent));
		sndok( (struct rf_msg_hdr *)reqptr,
		       (struct rf_msg_hdr *)resptr,
		       sizeof(struct rf_msg_rres) );
	}
	else {
		resptr->rf_len = htonl(0);
		sndok( (struct rf_msg_hdr *)reqptr,
		        (struct rf_msg_hdr *)resptr,
			sizeof(struct rf_msg_rres) );
	}

	return;
}
