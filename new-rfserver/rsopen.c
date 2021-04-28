/* rsopen.c - rsopen */

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
#include "file.h"

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

#ifdef DEBUG
	printf("DEBUG: reached rsopen\n");
#endif

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
		/* reset file position to 0 */
		if (sbuff.st_mode & S_IFDIR) {
			/* if directory, reset using seekdir() */
			seekdir((&ofiles[findex])->dirptr, 0);
		}

		resptr->rf_mode = reqptr->rf_mode;
		sndok ( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_ores) );
		return;
	}

	/* open the file or create if it does not exist */

	if (sreturn < 0) {	/* file does not exist */
#ifdef DEBUG
		printf("DEBUG: creating file %s\n",reqptr->rf_name);
#endif
		fd = rsofile(reqptr->rf_name, O_RDWR|O_CREAT);
	} else if (sbuff.st_mode & S_IFDIR) {
#ifdef DEBUG
		printf("DEBUG: opening directory %s\n", reqptr->rf_name);
#endif
		fd = rsodir(reqptr->rf_name);
	} else {
#ifdef DEBUG
		printf("DEBUG: opening old file %s\n",reqptr->rf_name);
#endif
		fd = rsofile(reqptr->rf_name, O_RDWR); // this needed to be moved out of the ifdef
	}

	/* if open failed or open file table is full, send error */

	if (fd < 0) {
#ifdef DEBUG
		printf("DEBUG: fd is %d and rsopen sends error\n", fd);
#endif
		resptr->rf_mode = reqptr->rf_mode;
		snderr( (struct rf_msg_hdr *)reqptr,
			(struct rf_msg_hdr *)resptr,
			 sizeof(struct rf_msg_ores) );
			return;
	}

	/* Return OK status */
#ifdef DEBUG
	printf("DEBUG: fd is %d and rsopen sends OK\n", fd);
#endif

	resptr->rf_mode = reqptr->rf_mode;
	sndok ( (struct rf_msg_hdr *)reqptr,
		(struct rf_msg_hdr *)resptr,
		 sizeof(struct rf_msg_ores) );
	return;
}
