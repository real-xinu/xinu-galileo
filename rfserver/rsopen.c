/* rsopen.c - rsopen */

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
 * rsopen - handle an open request
 *------------------------------------------------------------------------
 */
void	rsopen (
	 struct	rf_msg_oreq *reqptr,	/* ptr to read request		*/
	 struct	rf_msg_ores *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	int	i, j;			/* general loop indexes		*/
	struct	stat	buf;		/* buffer for file status	*/
	char	*to, *from;		/* used during copy		*/
	struct	stat	sbuff;		/* stat buffer			*/
	int32	modebits;		/* mode bits from message	*/
	int	sreturn;		/* stat return value		*/

/* DEBUG */ printf("DEBUG: reached rsopen\n");

	sreturn = stat(reqptr->rf_name, &sbuff);

	/* mode "n" means file must be new, "o" means it must be old */

	modebits = htonl(reqptr->rf_mode);

	if (modebits & RF_MODE_N) {
		if (sreturn >= 0) {	/* if file already exists */
			resptr->rf_mode = reqptr->rf_mode;
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				 sizeof(struct rf_msg_ores) );
			return;
		}
	} else if (modebits & RF_MODE_O) {
		if (sreturn < 0) {	/* if file does not exist */
			resptr->rf_mode = reqptr->rf_mode;
			snderr( (struct rf_msg_hdr *)reqptr,
				(struct rf_msg_hdr *)resptr,
				 sizeof(struct rf_msg_ores) );
			return;
		}
	}

	/* If file is already open, return OK status */

	if (findex >= 0) {
		resptr->rf_mode = reqptr->rf_mode;
		sndok ( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_ores) );
		return;
	}

	/* open the file or create if it does not exist */

	if (sreturn < 0) {	/* file does not exist */
/*DEBUG*/ printf("DEBUG: creating file %s\n",reqptr->rf_name);
		fd = rsofile(reqptr->rf_name, O_RDWR|O_CREAT);
	} else if (sbuff.st_mode & S_IFDIR) {
		printf("DEBUG: opening directory %s\n", reqptr->rf_name);
		fd = rsodir(reqptr->rf_name);
	} else {
/*DEBUG*/ printf("DEBUG: opening old file %s\n",reqptr->rf_name);
		fd = rsofile(reqptr->rf_name, O_RDWR);
	}

	/* if open failed or open file table is full, send error */

	if (fd < 0) {
/*DEBUG*/ printf("DEBUG: fd is %d and rsopen sends error\n", fd);
		resptr->rf_mode = reqptr->rf_mode;
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_ores) );
			return;
	}

	/* Return OK status */
/*DEBUG*/ printf("DEBUG: fd is %d and rsopen sends OK\n", fd);

	resptr->rf_mode = reqptr->rf_mode;
	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_ores) );
	return;
}
