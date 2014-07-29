/* lflRead.c  -  lfRead */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflRead  --  read from a previously opened local file
 *------------------------------------------------------------------------
 */
devcall	lflRead (
	  struct dentry *devptr,	/* entry in device switch table */
	  char	*buff,			/* buffer to hold bytes		*/
	  int32	count			/* max bytes to read		*/
	)
{
	uint32	numread;		/* number of bytes read		*/
	int32	nxtbyte;		/* character or SYSERR/EOF	*/

	if (count < 0) {
		return SYSERR;
	}

	for (numread=0 ; numread < count ; numread++) {
		nxtbyte = lflGetc(devptr);
		if (nxtbyte == SYSERR) {
			return SYSERR;
		} else if (nxtbyte == EOF) {	/* EOF before finished */
		    if (numread == 0) {
			return EOF;
		    } else {
			return numread;
		    }
		} else {
			*buff++ = (char) (0xff & nxtbyte);
		}
	}
	return numread;
}
