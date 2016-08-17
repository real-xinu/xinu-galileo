/* rdopen.c - rdopen */

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
 * rdopen - handle an open request
 *------------------------------------------------------------------------
 */
void	rdopen (
	 struct	rd_msg_oreq *reqptr,	/* ptr to read request		*/
	 struct	rd_msg_ores *resptr	/* ptr to read response		*/
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
	printf("DEBUG: reached rdopen\n");
#endif

	/* If disk is already open return SUCCESS */

	if (findex >= 0) {
		sndok ( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			 sizeof(struct rd_msg_ores) );
		return;
	}

	/* open the disk file or create if it does not exist */

	sreturn = stat(reqptr->rd_id, &sbuff);

	if (sreturn < 0) {	/* file does not exist */
#ifdef DEBUG
		printf("DEBUG: creating file %s\n", reqptr->rd_id);
#endif
		fd = rdofile(reqptr->rd_id, O_RDWR|O_CREAT);
	} else {
#ifdef DEBUG
		printf("DEBUG: opening old file %s\n",reqptr->rd_id);
#endif
		fd = rdofile(reqptr->rd_id, O_RDWR);
	}

	/* if open failed or open file table is full, send error */

	if (fd < 0) {
#ifdef DEBUG
		printf("DEBUG: fd is %d and rdopen sends error\n", fd);
#endif
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			 sizeof(struct rd_msg_ores) );
			return;
	}

	/* Return OK status */

	sndok ( (struct rd_msg_hdr *)reqptr,
		(struct rd_msg_hdr *)resptr,
		 sizeof(struct rd_msg_ores) );
	return;
}
