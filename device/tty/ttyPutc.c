/* ttyPutc.c - ttyPutc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyPutc - write one character to a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyPutc(
	struct	dentry	*devptr,	/* entry in device switch table	*/
	char	ch			/* character to write		*/
	)
{
	struct	ttycblk	*typtr;		/* pointer to tty control block	*/

	typtr = &ttytab[devptr->dvminor];

	/* Handle output CRLF by sending CR first */

        if ( ch==TY_NEWLINE && typtr->tyocrlf ) {
                ttyPutc(devptr, TY_RETURN);
	}

	wait(typtr->tyosem);		/* wait	for space in queue */
	*typtr->tyotail++ = ch;

	/* Wrap around to beginning of buffer, if needed */

	if (typtr->tyotail >= &typtr->tyobuff[TY_OBUFLEN]) {
		typtr->tyotail = typtr->tyobuff;
	}

	/* Start output in case device is idle */

	ttyKickOut(typtr, (struct uart_csreg *)devptr->dvcsr);

	return OK;
}
