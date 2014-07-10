/* ttyRead.c - ttyRead */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyRead - read character(s) from a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyRead(
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer of characters		*/
	  int32	count 			/* count of character to read	*/
	)
{
	struct	ttycblk	*typtr;		/* pointer to tty control block	*/
	int32	avail;			/* characters available in buff.*/
	int32	nread;			/* number of characters read	*/
	int32	firstch;		/* first input character on line*/
	char	ch;			/* next input character		*/

	if (count < 0) {
		return SYSERR;
	}
	typtr= &ttytab[devptr->dvminor];

	if (typtr->tyimode != TY_IMCOOKED) {

		/* For count of zero, return all available characters */

		if (count == 0) {
			avail = semcount(typtr->tyisem);
			if (avail == 0) {
				return 0;
			} else {
				count = avail;
			}
		}
		for (nread = 0; nread < count; nread++) {
			*buff++ = (char) ttyGetc(devptr);
		}
		return nread;
	}

	/* Block until input arrives */

	firstch = ttyGetc(devptr);

	/* Check for End-Of-File */

	if (firstch == EOF) {
		return (EOF);
	}

	/* read up to a line */

	ch = (char) firstch;
	*buff++ = ch;
	nread = 1;
	while ( (nread < count) && (ch != TY_NEWLINE) &&
			(ch != TY_RETURN) ) {
		ch = ttyGetc(devptr);
		*buff++ = ch;
		nread++;
	}
	return nread;
}
