/* ptyread.c - ttyread */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptyread  -  Read character(s) from a pty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ptyread(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of characters		*/
	  int32	count 			/* Count of character to read	*/
	)
{
	struct	ptycblk	*pyptr;		/* Pointer to tty control block	*/
	int32	avail;			/* Characters available in buff.*/
	int32	nread;			/* Number of characters read	*/
	int32	firstch;		/* First input character on line*/
	char	ch;			/* Next input character		*/

	if (count < 0) {
		return SYSERR;
	}
	pyptr= &ptytab[devptr->dvminor];

	if (pyptr->pyimode != PTY_IMCOOKED) {

		/* For count of zero, return all available characters */

		if (count == 0) {
			avail = semcount(pyptr->pyisem);
			if (avail == 0) {
				return 0;
			} else {
				count = avail;
			}
		}
		for (nread = 0; nread < count; nread++) {
			*buff++ = (char) ptygetc(devptr);
		}
		return nread;
	}

	/* Block until input arrives */

	firstch = ptygetc(devptr);

	/* Check for End-Of-File */

	if (firstch == EOF) {
		return EOF;
	}

	/* Read up to a line */

	ch = (char) firstch;
	*buff++ = ch;
	nread = 1;
	while ( (nread < count) && (ch != PTY_NEWLINE) &&
			(ch != PTY_RETURN) ) {
		ch = ptygetc(devptr);
		*buff++ = ch;
		nread++;
	}
	return nread;
}
