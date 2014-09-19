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
	volatile struct uart_csreg *regptr;

	devptr = (struct dentry *) &devtab[CONSOLE];
	regptr = (struct uart_csreg *)devptr->dvcsr;

	/* Fail if no console device was found */
	if (regptr == NULL) {
		return SYSERR;
	}

	/* Repeatedly poll the device until it becomes nonbusy */
	while ((regptr->lsr & UART_LSR_THRE) == 0) {
		;
	}

	/* Write the character */
	regptr->buffer = c;

	/* Honor CRLF - when writing NEWLINE also send CARRIAGE RETURN	*/
	if (c == '\n') {
		/* Poll until transmitter queue is empty */
		while ((regptr->lsr & UART_LSR_THRE) == 0) {
			;
		}
		regptr->buffer = '\r';
	}
	return OK;
}

/*------------------------------------------------------------------------
 * kgetc - use polled I/O to read a character from the console serial line
 *------------------------------------------------------------------------
 */
syscall kgetc(void)
{
	int irmask;
	volatile struct uart_csreg *regptr;
	byte c;
	struct	dentry	*devptr;

	devptr = (struct dentry *) &devtab[CONSOLE];
	regptr = (struct uart_csreg *)devptr->dvcsr;

	/* Fail if no console device was found */
	if (regptr == NULL) {
		return SYSERR;
	}

	irmask = regptr->ier;		/* Save UART interrupt state.   */
	regptr->ier = 0;		/* Disable UART interrupts.     */

	while (0 == (regptr->lsr & UART_LSR_DR)) {
		; /* Do Nothing */
	}

	/* Read character from Receive Holding Register */
	c = regptr->rbr;
	regptr->ier = irmask;		/* Restore UART interrupts.     */
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
