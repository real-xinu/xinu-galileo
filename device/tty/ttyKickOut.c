/* ttyKickOut.c - ttyKickOut */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyKickOut  -  "kick" the hardware for a tty device, causing it to
 *		   generate an output interrupt (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ttyKickOut(
	 struct	ttycblk	*typtr,		/* Ptr to ttytab entry		*/
	 struct uart_csreg *csrptr	/* Address of UART's CSRs	*/
	)
{
	/* Set the device to force it to generate an output interrupt	*/

	csrptr->ier = UART_IER_ERBFI | UART_IER_ETBEI;

	return;
}
