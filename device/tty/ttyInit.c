/* ttyInit.c - ttyInit */

#include <xinu.h>

struct	ttycblk	ttytab[Ntty];

/*------------------------------------------------------------------------
 *  ttyInit - initialize buffers and modes for a tty line
 *------------------------------------------------------------------------
 */
devcall	ttyInit(
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct	ttycblk	*typtr;		/* pointer to ttytab entry	*/
	struct	uart_csreg *uptr;	/* address of UART's CSRs	*/
	uint32	pciinfo;		/* PCI info to read config	*/

	typtr = &ttytab[ devptr->dvminor ];

	/* Initialize values in the tty control block */

	typtr->tyihead = typtr->tyitail = 	/* set up input queue	*/
		&typtr->tyibuff[0];		/*    as empty		*/
	typtr->tyisem = semcreate(0);		/* input semaphore	*/
	typtr->tyohead = typtr->tyotail = 	/* set up output queue	*/
		&typtr->tyobuff[0];		/*    as empty		*/
	typtr->tyosem = semcreate(TY_OBUFLEN);	/* output semaphore	*/
	typtr->tyehead = typtr->tyetail = 	/* set up echo queue	*/
		&typtr->tyebuff[0];		/*    as empty		*/
	typtr->tyimode = TY_IMCOOKED;		/* start in cooked mode	*/
	typtr->tyiecho = TRUE;			/* echo console input	*/
	typtr->tyieback = TRUE;			/* honor erasing bksp	*/
	typtr->tyevis = TRUE;			/* visual control chars	*/
	typtr->tyecrlf = TRUE;			/* echo CRLF for NEWLINE*/
	typtr->tyicrlf = TRUE;			/* map CR to NEWLINE	*/
	typtr->tyierase = TRUE;			/* do erasing backspace	*/
	typtr->tyierasec = TY_BACKSP;		/* erase char is ^H	*/
	typtr->tyeof = TRUE;			/* honor eof on input	*/
	typtr->tyeofch = TY_EOFCH;		/* end-of-file character*/
	typtr->tyikill = TRUE;			/* allow line kill	*/
	typtr->tyikillc = TY_KILLCH;		/* set line kill to ^U	*/
	typtr->tyicursor = 0;			/* start of input line	*/
	typtr->tyoflow = TRUE;			/* handle flow control	*/
	typtr->tyoheld = FALSE;			/* output not held	*/
	typtr->tyostop = TY_STOPCH;		/* stop char is ^S	*/
	typtr->tyostart = TY_STRTCH;		/* start char is ^Q	*/
	typtr->tyocrlf = TRUE;			/* send CRLF for NEWLINE*/
	typtr->tyifullc = TY_FULLCH;		/* send ^G when buffer	*/
						/*   is full		*/

	/* Read PCI config space to get memory base address */

	pciinfo = find_pci_device(INTEL_QUARK_UART_PCI_DID, INTEL_QUARK_UART_PCI_VID, 1);
	if((int)pciinfo == SYSERR) {
		return SYSERR;
	}

	pci_read_config_dword(pciinfo, 0x10, (uint32 *)&devptr->dvcsr);

	/* Initialize the UART */

	uptr = (struct uart_csreg *)devptr->dvcsr;

	/* Set baud rate */
	uptr->lcr = UART_LCR_DLAB;
	uptr->dll = 0x00;
	uptr->dlm = 0x00;
	uptr->dll = 0x18;

	uptr->lcr = UART_LCR_8N1;		/* 8 bit char, No Parity, 1 Stop*/
	uptr->fcr = 0x00;			/* Disable FIFO for now		*/

	/* OUT2 value is used to control the onboard interrupt tri-state*/
	/* buffer. It should be set high to generate interrupts		*/
	//outb((int)&uptr->mcr, UART_MCR_DTR | 
	//		      UART_MCR_RTS | 
	//		      UART_MCR_OUT2);	/* Turn on user-defined OUT2	*/

	/* Register the interrupt dispatcher for the tty device */

	set_evec( devptr->dvirq, (uint32)devptr->dvintr );

	/* Enable interrupts on the device */

	/* Enable UART FIFOs, clear and set interrupt trigger level	*/
	uptr->fcr = UART_FCR_EFIFO | UART_FCR_RRESET |
			UART_FCR_TRESET | UART_FCR_TRIG2;

	ttyKickOut(typtr, uptr);

	return OK;
}
