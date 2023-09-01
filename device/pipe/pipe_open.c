/* pipeopen.c - pipeopen */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  pipeopen  -  open one of the pipe pseudo devices
 *------------------------------------------------------------------------
 */
devcall	pipe_open(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Not used			*/
	  int32	count 			/* Not used			*/
	)
{
	struct	pipecblk *piptr;	/* Pointer to pipe control block*/
	int	i;			/* Walks through control blocks	*/

	/* Find a pipe pseudo device that is available */

	for (i=0; i<Npip; i++) {
		piptr = &pipetab[i];
		
		if (piptr->pstate == PIPE_FREE) {
			break;
		}
	}
	if (i >= Npip) {
		return SYSERR;
	}

	piptr->pstate = PIPE_OPEN;
	piptr->pcsem = semcreate(0);
	piptr->ppsem = semcreate(PIPE_BUF_SIZE);
	piptr->phead = piptr->ptail = 0;
	piptr->pavail= 0;
	return piptr->pdevid;
}
