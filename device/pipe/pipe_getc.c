/* pipe_getc.c - pipe_getc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  pipe_getc  -  read one character from a pipe device
 *------------------------------------------------------------------------
 */
devcall	pipe_getc(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	char	ch;			/* Byte of data from the buffer	*/
	struct	pipecblk *piptr;	/* Pointer to pipe control block*/

	/* Get a pointer to the control block for this pipe */

	piptr = &pipetab[devptr->dvminor];

	/* Check if pipe is not in use or at EOF */

	if (piptr->pstate == PIPE_FREE) {
		/* The pipe is not available */
		return SYSERR;
	}
	if (piptr->pstate == PIPE_EOF) {

		/* The writer closed the pipe, so return bytes while	*/
		/*  any remain in the buffer.				*/

		if (piptr->pavail > 0) {
			ch = piptr->pbuf[piptr->phead++];
			if (piptr->phead >= PIPE_BUF_SIZE) {
				piptr->phead = 0;
			}
			piptr->pavail--;
			return 0xff & ch;
		}
		return EOF;
	}

	/* State is OPEN -- Wait for a byte to be available or a close	*/

	wait(piptr->pcsem);

	/* If the state changed while we were blocked, the producer must*/
	/*  have called close, possibly after writing bytes to the pipe.*/

	if (piptr->pstate == PIPE_EOF) {
		if (piptr->pavail <= 0) {
			/* The buffer is empty */
			return EOF;
		}
	}

	/* A byte is available to be read -- pick up and return the byte*/

	ch = piptr->pbuf[piptr->phead++];
	if (piptr->phead >= PIPE_BUF_SIZE) {
		piptr->phead = 0;
	}
	piptr->pavail--;

	/* Signal the producer and return the byte */

	signal(piptr->ppsem);
	return 0xff & ch;
}
