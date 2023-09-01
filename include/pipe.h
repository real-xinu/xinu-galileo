/* pipe.h */

/* Definitions for a pipe device */

#ifndef PIPE_BUF_SIZE
#define	PIPE_BUF_SIZE	20	/* Default size of a pipe buffer	*/
#endif

/* state constants for a pipe pseudo device */

#define	PIPE_FREE	0	/* Entry is not currently used		*/
#define	PIPE_OPEN	1	/* Entry is open for reading and writing*/
#define	PIPE_EOF	2	/* Entry has been closed for writes, but*/
				/*  remains open for reading until all	*/
				/*  chars have been read and EOF has	*/
				/*  returned				*/

struct	pipecblk {
	int32	pstate;		/* State of this pipe device		*/
	byte	pbuf[PIPE_BUF_SIZE]; /* Buffer for the pipe		*/
	int32	phead;		/* Index of next byte in pbuf to read	*/
	int32	ptail;		/* Index of next byte in pbuf to write	*/
	sid32	ppsem;		/* Producer semaphore for the pipe	*/
	sid32	pcsem;		/* Consumer semaphore for the pipe	*/
	did32	pdevid;		/* Device ID of this pseudo device	*/
	int32	pavail;		/* Available characters during drain	*/
};

extern	struct	pipecblk pipetab[];
