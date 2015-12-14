/* ttycontrol.c - ttycontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptycontrol  -  Control a pty device by setting modes
 *------------------------------------------------------------------------
 */
devcall	ptycontrol(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  int32	 func,			/* Function to perform		*/
	  int32	 arg1,			/* Argument 1 for request	*/
	  int32	 arg2			/* Argument 2 for request	*/
	)
{
	struct	ptycblk	*pyptr;		/* Pointer to tty control block	*/
	char	ch;			/* Character for lookahead	*/

	pyptr = &ptytab[devptr->dvminor];

	/* Process the request */

	switch ( func )	{

	case PC_NEXTC:
		wait(pyptr->pyisem);
		ch = *pyptr->pyitail;
		signal(pyptr->pyisem);
		return (devcall)ch;

	case PC_MODER:
		pyptr->pyimode = PTY_IMRAW;
		return (devcall)OK;

	case PC_MODEC:
		pyptr->pyimode = PTY_IMCOOKED;
		return (devcall)OK;

	case PC_MODEK:
		pyptr->pyimode = PTY_IMCBREAK;
		return (devcall)OK;

	case PC_ICHARS:
		return(semcount(pyptr->pyisem));

	case PC_ECHO:
		pyptr->pyiecho = TRUE;
		return (devcall)OK;

    case PC_NOECHO:
        pyptr->pyiecho = FALSE;
        return (devcall)OK;

    case PC_OUTPUT_FUNC:
        pyptr->out = (OUT_PTY)arg1;
        pyptr->arg_out = (void *)arg2;
        return (devcall)OK;

    case PC_RESET:
        pyptr->pyihead = pyptr->pyitail = 	/* Set up input queue	*/
            &pyptr->pyibuff[0];		/*    as empty		*/
        semreset(pyptr->pyisem, 0);		/* Input semaphore	*/
        pyptr->pyimode = PTY_IMCOOKED;		/* Start in cooked mode	*/
        pyptr->pyiecho = TRUE;			/* Echo console input	*/
        pyptr->pyieback = TRUE;			/* Honor erasing bksp	*/
        pyptr->pyevis = TRUE;			/* Visual control chars	*/
        pyptr->pyecrlf = TRUE;			/* Echo CRLF for NEWLINE*/
        pyptr->pyicrlf = TRUE;			/* Map CR to NEWLINE	*/
        pyptr->pyierase = TRUE;			/* Do erasing backspace	*/
        pyptr->pyierasec = PTY_BACKSP;		/* Primary erase char	*/
        pyptr->pyierasec2= PTY_BACKSP2;		/* Alternate erase char	*/
        pyptr->pyeof = TRUE;			/* Honor eof on input	*/
        pyptr->pyeofch = PTY_EOFCH;		/* End-of-file character*/
        pyptr->pyikill = TRUE;			/* Allow line kill	*/
        pyptr->pyikillc = PTY_KILLCH;		/* Set line kill to ^U	*/
        pyptr->pyicursor = 0;			/* Start of input line	*/
        pyptr->pyoflow = TRUE;			/* Handle flow control	*/
        pyptr->pyoheld = FALSE;			/* Output not held	*/
        pyptr->pyostop = PTY_STOPCH;		/* Stop char is ^S	*/
        pyptr->pyostart = PTY_STRTCH;		/* Start char is ^Q	*/
        pyptr->pyocrlf = TRUE;			/* Send CRLF for NEWLINE*/
        pyptr->pyifullc = PTY_FULLCH;		/* Send ^G when buffer	*/
        /*   is full		*/

        /* Call back mechanism */
        pyptr->out = (OUT_PTY) SYSERR;
        pyptr->arg_out = (void *) SYSERR;
        return (devcall)OK;

    default:
        return (devcall)SYSERR;
    }
}
