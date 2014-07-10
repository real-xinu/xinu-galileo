/* ttyControl.c - ttyControl */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyControl  -  control a tty device by setting modes
 *------------------------------------------------------------------------
 */
devcall	ttyControl(
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  int32	 func,			/* function to perform		*/
	  int32	 arg1,			/* argument 1 for request	*/
	  int32	 arg2			/* argument 2 for request	*/
	)
{
	struct	ttycblk	*typtr;		/* pointer to tty control block	*/
	char	ch;			/* character for lookahead	*/

	typtr = &ttytab[devptr->dvminor];

	/* Process the request */

	switch ( func )	{

	case TC_NEXTC:
		wait(typtr->tyisem);
		ch = *typtr->tyitail;
		signal(typtr->tyisem);
		return (devcall)ch;

	case TC_MODER:
		typtr->tyimode = TY_IMRAW;
		return (devcall)OK;

	case TC_MODEC:
		typtr->tyimode = TY_IMCOOKED;
		return (devcall)OK;

	case TC_MODEK:
		typtr->tyimode = TY_IMCBREAK;
		return (devcall)OK;

	case TC_ICHARS:
		return(semcount(typtr->tyisem));

	case TC_ECHO:
		typtr->tyiecho = TRUE;
		return (devcall)OK;

	case TC_NOECHO:
		typtr->tyiecho = FALSE;
		return (devcall)OK;

	default:
		return (devcall)SYSERR;
	}
}
