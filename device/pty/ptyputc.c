/* ptyputc.c - ptyputc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptyputc  -  Write one character to a pty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ptyputc(
        struct	dentry	*devptr,	/* Entry in device switch table	*/
        char	ch			/* Character to write		*/
        )
{
    struct	ptycblk	*pyptr;		/* Pointer to pty control block	*/
    byte    buff[2];
    int32   idx;

    pyptr = &ptytab[devptr->dvminor];
    idx = 0;

    /* Handle output CRLF by sending CR first */

    if (ch == PTY_NEWLINE && pyptr->pyocrlf ) {
        buff[idx++] = TY_RETURN;
    }
    buff[idx++] = ch;

    /* call output function */
    if (pyptr->out != (OUT_PTY) SYSERR) {
        (pyptr->out)(pyptr->arg_out, buff, idx);
    }

    return OK;
}
