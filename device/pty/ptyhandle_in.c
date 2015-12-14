/* ptyhandle_in.c - ptyhandle_in, erase1, eputc, echoch */

#include <xinu.h>

local	void	erase1(struct ptycblk *);
local	void	echoch(char, struct ptycblk *);
local	void	eputc(char*, int32, struct ptycblk *);

/*------------------------------------------------------------------------
 *  ptyhandle_in  -  Handle one arriving char (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ptyhandle_in (
        struct ptycblk *pyptr,	/* Pointer to ptytab entry	*/
        char    ch              /* Next char from device	*/
        )
{

    char buff[2];
    int32 avail;

    /* Compute chars available */
    avail = semcount(pyptr->pyisem);
    if (avail < 0) {		/* One or more processes waiting*/
        avail = 0;
    }

    /* Handle raw mode */
    if (pyptr->pyimode == PTY_IMRAW) {
        if (avail >= PTY_IBUFLEN) { /* No space => ignore input	*/
            return;
        }

        /* Place char in buffer with no editing */
        *pyptr->pyitail++ = ch;

        /* Wrap buffer pointer	*/
        if (pyptr->pyitail >= &pyptr->pyibuff[PTY_IBUFLEN]) {
            pyptr->pyitail = pyptr->pyibuff;
        }

        /* Signal input semaphore and return */
        signal(pyptr->pyisem);
        return;
    }

    /* Handle cooked and cbreak modes (common part) */
    if ( (ch == PTY_RETURN) && pyptr->pyicrlf ) {
        ch = PTY_NEWLINE;
    }

    /* If flow control is in effect, handle ^S and ^Q */
    if (pyptr->pyoflow) {
        if (ch == pyptr->pyostart) {	    /* ^Q starts output	*/
            pyptr->pyoheld = FALSE;
            // ptykickout(csrptr);
            return;
        } else if (ch == pyptr->pyostop) {  /* ^S stops	output	*/
            pyptr->pyoheld = TRUE;
            return;
        }
    }

    pyptr->pyoheld = FALSE;		/* Any other char starts output */

    if (pyptr->pyimode == PTY_IMCBREAK) {	   /* Just cbreak mode	*/

        /* If input buffer is full, send bell to user */
        if (avail >= PTY_IBUFLEN) {
            buff[0] = pyptr->pyifullc;
            eputc(buff, 1, pyptr);
        } else {	/* Input buffer has space for this char */
            *pyptr->pyitail++ = ch;

            /* Wrap around buffer */

            if (pyptr->pyitail>=&pyptr->pyibuff[PTY_IBUFLEN]) {
                pyptr->pyitail = pyptr->pyibuff;
            }
            if (pyptr->pyiecho) {	/* Are we echoing chars?*/
                echoch(ch, pyptr);
            }
        }
        return;

    } else {	/* Just cooked mode (see common code above) */

        /* Line kill character arrives - kill entire line */
        if (ch == pyptr->pyikillc && pyptr->pyikill) {
            pyptr->pyitail -= pyptr->pyicursor;
            if (pyptr->pyitail < pyptr->pyibuff) {
                pyptr->pyihead += PTY_IBUFLEN;
            }
            pyptr->pyicursor = 0;
            buff[0] = PTY_RETURN;
            buff[1] = PTY_NEWLINE;
            eputc(buff, 2, pyptr);
            return;
        }

        /* Erase (backspace) character */

        if ( ((ch==pyptr->pyierasec) || (ch==pyptr->pyierasec2))
                && pyptr->pyierase) {
            if (pyptr->pyicursor > 0) {
                pyptr->pyicursor--;
                erase1(pyptr);
            }
            return;
        }

        /* End of line */

        if ( (ch == PTY_NEWLINE) || (ch == PTY_RETURN) ) {
            if (pyptr->pyiecho) {
                echoch(ch, pyptr);
            }
            *pyptr->pyitail++ = ch;
            if (pyptr->pyitail>=&pyptr->pyibuff[PTY_IBUFLEN]) {
                pyptr->pyitail = pyptr->pyibuff;
            }
            /* Make entire line (plus \n or \r) available */
            signaln(pyptr->pyisem, pyptr->pyicursor + 1);
            pyptr->pyicursor = 0; 	/* Reset for next line	*/
            return;
        }

        /* Character to be placed in buffer - send bell if	*/
        /*	buffer has overflowed				*/

        avail = semcount(pyptr->pyisem);
        if (avail < 0) {
            avail = 0;
        }
        if ((avail + pyptr->pyicursor) >= PTY_IBUFLEN-1) {
            buff[0] = pyptr->pyifullc;
            eputc(buff, 1, pyptr);
            return;
        }

        /* EOF character: recognize at beginning of line, but	*/
        /*	print and ignore otherwise.			*/

        if (ch == pyptr->pyeofch && pyptr->pyeof) {
            if (pyptr->pyiecho) {
                echoch(ch, pyptr);
            }
            if (pyptr->pyicursor != 0) {
                return;
            }
            *pyptr->pyitail++ = ch;
            signal(pyptr->pyisem);
            return;
        }


        /* Echo the character */
        if (pyptr->pyiecho) {
            echoch(ch, pyptr);
        }

        /* Insert in the input buffer */
        pyptr->pyicursor++;
        *pyptr->pyitail++ = ch;

        /* Wrap around if needed */

        if (pyptr->pyitail >= &pyptr->pyibuff[PTY_IBUFLEN]) {
            pyptr->pyitail = pyptr->pyibuff;
        }
        return;
    }
}

/*------------------------------------------------------------------------
 *  erase1  -  Erase one character honoring erasing backspace
 *------------------------------------------------------------------------
 */
local	void	erase1(
        struct ptycblk	*pyptr	/* Ptr to ptytab entry		*/
        )
{
    char	ch;			/* Character to erase		*/
    char buff[6];
    int32 idx;

    if ( (--pyptr->pyitail) < pyptr->pyibuff) {
        pyptr->pyitail += PTY_IBUFLEN;
    }
    idx = 0;

    /* Pick up char to erase */
    ch = *pyptr->pyitail;
    if (pyptr->pyiecho) {			   /* Are we echoing?	*/
        if (ch < PTY_BLANK || ch == 0177) { /* Nonprintable	*/
            if (pyptr->pyevis) {	/* Visual cntl chars */
                buff[idx++] = PTY_BACKSP;
                if (pyptr->pyieback) { /* Erase char	*/
                    buff[idx++] = PTY_BLANK;
                    buff[idx++] = PTY_BACKSP;
                }
            }
            buff[idx++] = PTY_BACKSP;/* Bypass up arr*/
            if (pyptr->pyieback) {
                buff[idx++] = PTY_BLANK;
                buff[idx++] = PTY_BACKSP;
            }
            eputc(buff, idx, pyptr);
        } else {  /* A normal character that is printable	*/
            buff[idx++] = PTY_BACKSP;
            if (pyptr->pyieback) { /* Erase character */
                buff[idx++] = PTY_BLANK;
                buff[idx++] = PTY_BACKSP;
            }
            eputc(buff, idx, pyptr);
        }
    }
    return;
}

/*------------------------------------------------------------------------
 *  echoch  -  Echo a character with visual and output crlf options
 *------------------------------------------------------------------------
 */
local	void	echoch(
        char	ch,			/* Character to	echo		*/
        struct ptycblk *pyptr	/* Ptr to ptytab entry		*/
        )
{
    char buff[2];

    if ((ch==PTY_NEWLINE || ch==PTY_RETURN) && pyptr->pyecrlf) {
        buff[0] = PTY_RETURN;
        buff[1] = PTY_NEWLINE;
        eputc(buff, 2, pyptr);
    } else if ( (ch<PTY_BLANK||ch==0177) && pyptr->pyevis) {
        buff[0] = PTY_UPARROW;/* print ^x		*/
        buff[1] = ch+0100;	/* Make it printable	*/
        eputc(buff, 2, pyptr);
    } else {
        buff[0] = ch;
        eputc(buff, 1, pyptr);
    }
}

/*------------------------------------------------------------------------
 *  eputc  -  Echo characters
 *------------------------------------------------------------------------
 */
local	void	eputc(
        char*	ch,			/* Character to	echo		*/
        int32   length,
        struct ptycblk *pyptr	/* Ptr to ptytab entry		*/
        )
{

    if (pyptr->out != (OUT_PTY) SYSERR) {
        (pyptr->out)(pyptr->arg_out, ch, length);
    }

    return;
}
