/* pipeputc.c - pipeputc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  pipeputc  -  write one character to a pipe device
 *------------------------------------------------------------------------
 */
devcall	pipe_putc(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	ch			/* Byte to write		*/
	)
{
	struct	pipecblk *piptr;	/* Pointer to pipe control block*/

	/* Get a pointer to the control block for this pipe */

	piptr = &pipetab[devptr->dvminor];

	/* Check that the pipe is available for writing */

	if (piptr->pstate != PIPE_OPEN) {
		return SYSERR;
	}

	wait(piptr->ppsem);

	/* See if pipe was closed or set to EOF while we were blocked	*/
	
	if (piptr->pstate != PIPE_OPEN) {
		return SYSERR;
	}

	/* Deposit a byte in next buffer position */

	piptr->pbuf[piptr->ptail++] = ch;
	if (piptr->ptail >= PIPE_BUF_SIZE) {
		piptr->ptail = 0;
	}
	piptr->pavail++;

	/* Signal the consumer semaphore and return */

	signal(piptr->pcsem);
	return OK;
}
