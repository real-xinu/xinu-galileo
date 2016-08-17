/* rdread.c - rdread */

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
 * rdread - handle a read request
 *------------------------------------------------------------------------
 */
void	rdread (
	 struct	rd_msg_rreq *reqptr,	/* ptr to read request		*/
	 struct	rd_msg_rres *resptr	/* ptr to read response		*/
	)
{
	int	fd;			/* file descriptor		*/
	int	i, j;			/* general loop indexes		*/
	struct	stat	buf;		/* buffer for file status	*/
	int	retval;			/* function return value	*/
	char	*to, *from;		/* used during copy		*/
	int	offset;			/* offset of block in file	*/
	int	nbytes;			/* num. of bytes read		*/

	/* if disk is not open, return error */

	if (findex < 0) {
#ifdef DEBUG
		printf("read from disk that is not open\n");
#endif
		resptr->rd_blk = reqptr->rd_blk;
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			sizeof(struct rd_msg_rreq) );
			return;
	}

	/* read block from file */

	fd = fptr->desc;

	/* stat file to get its current size */

	retval = fstat(fd, &buf);

	offset = ntohl(reqptr->rd_blk) * RD_BLKSIZ;

	/* if requested block is beyond EOF, return error */

	if ( (retval<0) || (offset > buf.st_size) ) {
#ifdef DEBUG
		printf("read from beyond EOF\n");
#endif
		resptr->rd_blk = reqptr->rd_blk;
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			sizeof(struct rd_msg_rreq) );
		return;
	}

	/* seek to specified offset and read data */

	lseek(fd, offset, SEEK_SET);
	nbytes = read(fd, resptr->rd_data, RD_BLKSIZ);
#ifdef DEBUG
	printf("DEBUG: read from desc %d specified file returns %d \n",
			fd, nbytes);
#endif
	
	if (nbytes != RD_BLKSIZ) {
#ifdef DEBUG
		printf("read does not return a block (%d bytes)\n", 
				nbytes);
#endif
		resptr->rd_blk = reqptr->rd_blk;
		snderr( (struct rd_msg_hdr *)reqptr,
			(struct rd_msg_hdr *)resptr,
			sizeof(struct rd_msg_rreq) );
		return;
	}

	resptr->rd_blk = reqptr->rd_blk;
	sndok(	(struct rd_msg_hdr *)reqptr,
		(struct rd_msg_hdr *)resptr,
		sizeof(struct rd_msg_rres) );
	return;
}
