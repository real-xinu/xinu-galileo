/* ptygetc.c - ptygetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptygetc  -  Read one character from a pty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ptygetc(
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	char	ch;			/* Character to return		*/
	struct	ptycblk	*pyptr;		/* Pointer to ptytab entry	*/

	pyptr = &ptytab[devptr->dvminor];

	/* Wait for a character in the buffer and extract one character	*/

	wait(pyptr->pyisem);
	ch = *pyptr->pyihead++;

	/* Wrap around to beginning of buffer, if needed */

	if (pyptr->pyihead >= &pyptr->pyibuff[PTY_IBUFLEN]) {
		pyptr->pyihead = pyptr->pyibuff;
	}

	/* In cooked mode, check for the EOF character */

	if ( (pyptr->pyimode == PTY_IMCOOKED) && (pyptr->pyeof) &&
	     (ch == pyptr->pyeofch) ) {
		return (devcall)EOF;
	}

	return (devcall)ch;
}
