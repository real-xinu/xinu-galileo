/* ttyGetc.c - ttyGetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyGetc - read one character from a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyGetc(
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	char	ch;
	struct	ttycblk	*typtr;		/* pointer to ttytab entry	*/

	typtr = &ttytab[devptr->dvminor];

	/* Wait for a character in the buffer */

	wait(typtr->tyisem);
	ch = *typtr->tyihead++;		/* extract one character	*/

	/* Wrap around to beginning of buffer, if needed */

	if (typtr->tyihead >= &typtr->tyibuff[TY_IBUFLEN]) {
		typtr->tyihead = typtr->tyibuff;
	}

	if ( (typtr->tyimode == TY_IMCOOKED) && (typtr->tyeof) &&
	     (ch == typtr->tyeofch) ) {
		return (devcall)EOF;
	}

	return (devcall)ch;
}
