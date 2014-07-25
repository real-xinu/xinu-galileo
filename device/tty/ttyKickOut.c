/* ttyKickOut.c - ttyKickOut */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyKickOut  -  "Kick" the hardware for a tty device, causing it to
 *		     generate an output interrupt (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ttyKickOut(
	 struct uart_csreg *csrptr	/* Address of UART's CSRs	*/
	)
{
	/* Set the device to force it to generate an interrupt */

	csrptr->ier = UART_IER_ERBFI | UART_IER_ETBEI;

	return;
}
