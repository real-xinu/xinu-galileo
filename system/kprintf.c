/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>


/*------------------------------------------------------------------------
 * kputc  -  use polled I/O to write a character to the console
 *------------------------------------------------------------------------
 */
syscall kputc(byte c)	/* Character to write	*/
{
	struct	dentry	*devptr;
	volatile struct uart_csreg *csrptr;
	intmask	mask;

	/* Disable interrupts */
	mask = disable();

	devptr = (struct dentry *) &devtab[CONSOLE];
	csrptr = (struct uart_csreg *)devptr->dvcsr;

	/* Fail if no console device was found */
	if (csrptr == NULL) {
		restore(mask);
		return SYSERR;
	}

	/* Repeatedly poll the device until it becomes nonbusy */
	while ((csrptr->lsr & UART_LSR_THRE) == 0) {
		;
	}

	/* Write the character */
	csrptr->buffer = c;

	/* Honor CRLF - when writing NEWLINE also send CARRIAGE RETURN	*/
	if (c == '\n') {
		/* Poll until transmitter queue is empty */
		while ((csrptr->lsr & UART_LSR_THRE) == 0) {
			;
		}
		csrptr->buffer = '\r';
	}

	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * kgetc - use polled I/O to read a character from the console serial line
 *------------------------------------------------------------------------
 */
syscall kgetc(void)
{
	int irmask;
	volatile struct uart_csreg *csrptr;
	byte c;
	struct	dentry	*devptr;
	intmask	mask;

	/* Disable interrupts */
	mask = disable();

	devptr = (struct dentry *) &devtab[CONSOLE];
	csrptr = (struct uart_csreg *)devptr->dvcsr;

	/* Fail if no console device was found */
	if (csrptr == NULL) {
		restore(mask);
		return SYSERR;
	}

	irmask = csrptr->ier;		/* Save UART interrupt state.   */
	csrptr->ier = 0;		/* Disable UART interrupts.     */

	/* wait for UART transmit queue to empty */

	while (0 == (csrptr->lsr & UART_LSR_DR)) {
		; /* Do Nothing */
	}

	/* Read character from Receive Holding Register */

	c = csrptr->rbr;
	csrptr->ier = irmask;		/* Restore UART interrupts.     */

	restore(mask);
	return c;
}

extern	void	_doprnt(char *, va_list ap, int (*)(int));

/*------------------------------------------------------------------------
 * kprintf  -  use polled I/O to print formatted output on the console
 *------------------------------------------------------------------------
 */
syscall kprintf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_doprnt(fmt, ap, (int (*)(int))kputc);
	va_end(ap);
	return OK;
}
