/* pipe_init.c  -  pipe_init */

#include <xinu.h>

struct	pipecblk pipetab[Npip];

/*------------------------------------------------------------------------
 * pipe_init  -  initialize a pipe pseudo device
 *------------------------------------------------------------------------
 */
devcall	pipe_init (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	pipecblk *piptr;	/* Pointer to pipe control block*/

	piptr = &pipetab[devptr->dvminor];

	piptr->pstate = PIPE_FREE;
	piptr->pdevid = devptr->dvnum;
	return OK;
}	
