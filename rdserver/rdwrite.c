/* rdwrite.c - rdwrite */

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
 * rdwrite - handle a write request
 *------------------------------------------------------------------------
 */
void	rdwrite (
	 struct	rd_msg_wreq *reqptr,	/* ptr to read request		*/
	 struct	rd_msg_wres *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	int	retval;			/* function return value	*/
	int	nbytes;			/* num. of bytes written to file*/
	int	offset;			/* offset of block in file	*/
	int	i;			/* index of ofiles table	*/
	char	*from, *to;		/* used during name copy	*/

	/* if disk is not open return an error */

	if (findex < 0) {
#ifdef DEBUG
	printf("disk is not open \n");
#endif
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			sizeof(struct rd_msg_wres) );
			return;
	}

	/* get file descriptor to used */

	fd = ofiles[findex].desc;

	/* seek to specified offset and write data */

	offset = ntohl(reqptr->rd_blk) * RD_BLKSIZ;
	lseek(fd, offset, SEEK_SET);

	nbytes = write(fd, reqptr->rd_data, RD_BLKSIZ);
#ifdef DEBUG
	printf("write of blk %d returns %d \n", 
			ntohl(reqptr->rd_blk), nbytes);
#endif
	if (nbytes != RD_BLKSIZ) {
#ifdef DEBUG
		printf("DEBUG: write fails\n");
#endif
		resptr->rd_blk = reqptr->rd_blk;
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			sizeof(struct rd_msg_wres) );
		return;
	}

	/* Return success */

	resptr->rd_blk = reqptr->rd_blk;
	sndok(	(struct rd_msg_hdr *)reqptr,
		(struct rd_msg_hdr *)resptr,
		sizeof(struct rd_msg_wres) );
	return;
}
