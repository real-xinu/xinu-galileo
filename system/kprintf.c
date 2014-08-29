/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * kputc  -  use polled I/O to write a character to the console
 *------------------------------------------------------------------------
 */

#define QUARK_CONS_PORT		1		/* Index of console UART */
#define QUARK_CONS_BAR_INDEX	0		/* Index of console's MMIO 
						   base address register */

/**
 *  console_init()
 *
 * Initialize the serial console.  The serial console is on the 
 * second memory-mapped 16550 UART device. 
 */
int console_init(void)
{
	int	status;
	int	pciDev;

	pciDev = find_pci_device(INTEL_QUARK_UART_PCI_DID,
				 INTEL_QUARK_UART_PCI_VID,
				 QUARK_CONS_PORT);
	if (pciDev < 0) {
		/* Error finding console device */
		return	pciDev;
	}
	/* Store the console device CSR base address into the console device's
	   device table entry. */
	status = pci_get_dev_mmio_base_addr(pciDev, QUARK_CONS_BAR_INDEX,
					   &devtab[CONSOLE].dvcsr);
	return status;
}

syscall kputc(byte c)	/* Character to write	*/
{
	struct	dentry	*devptr;
	volatile struct uart_csreg *regptr;

	devptr = (struct dentry *) &devtab[CONSOLE];
	regptr = (struct uart_csreg *)devptr->dvcsr;

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
