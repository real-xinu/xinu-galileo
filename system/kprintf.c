/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * kputc  -  use polled I/O to write a character to the console
 *------------------------------------------------------------------------
 */

// teshack - temporary shim to enable kprintf via Quark memory-mapped 16550s
#define XMTRDY	0x00000020
static void *pclnuart = (void *) 0x9000b000;// 0x8010f000;
static inline unsigned int readl(const volatile void *addr)
{
	return *(const volatile unsigned int *) addr;
}
static inline void writel(unsigned int b, volatile void *addr)
{
	*(volatile unsigned int *) addr = b;
}

syscall kputc(
	  byte	c			/* Character to write		*/
	)
{
	unsigned char   	status;
	unsigned int		inreg = UART_LSR << 2;

	if (pclnuart == NULL) {
		return -1;
	}

	/* Repeatedly poll the device until it becomes nonbusy */
	while ( (readl(pclnuart + inreg) & UART_LSR_THRE) == 0 ) {
		;
	}

	while (1) {
		status = readl(pclnuart + inreg);
		if (status & XMTRDY) {
			break;
        }
	}

	/* Write the character */
	writel(c, pclnuart + (UART_TX << 2));

	/* Honor CRLF - when writing NEWLINE also send CARRIAGE RETURN	*/
	if (c == '\n') {
		/* Poll until transmitter queue is empty */
		while ( (readl(pclnuart + inreg) & UART_LSR_THRE) == 0 ) {
			;
		}
		writel('\r', pclnuart + (UART_TX << 2));
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

	irmask = regptr->ier;       /* Save UART interrupt state.   */
	regptr->ier = 0;            /* Disable UART interrupts.     */

	while (0 == (regptr->lsr & UART_LSR_DR)) {
		; /* Do Nothing */
	}

	/* Read character from Receive Holding Register */
	c = regptr->rbr;
	regptr->ier = irmask;       /* Restore UART interrupts.     */
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
