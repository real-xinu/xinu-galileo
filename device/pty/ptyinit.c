/* ptyinit.c - ptyinit */

#include <xinu.h>

struct	ptycblk	ptytab[Npty];

/*------------------------------------------------------------------------
 *  ptyinit  -  Initialize buffers and modes for a pty line
 *------------------------------------------------------------------------
 */
devcall	ptyinit(
        struct dentry	*devptr		/* Entry in device switch table	*/
        )
{
    struct	ptycblk	*pyptr;		/* Pointer to ttytab entry	*/

    pyptr = &ptytab[devptr->dvminor];

    /* Initialize values in the pty control block */

    pyptr->pyihead = pyptr->pyitail = 	/* Set up input queue	*/
        &pyptr->pyibuff[0];		/*    as empty		*/
    pyptr->pyisem = semcreate(0);		/* Input semaphore	*/
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

    return OK;
}
