/* pipe_read.c - pipe_read */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  pipe_read  -  Read one or more bytes of data from a pipe device
 *------------------------------------------------------------------------
 */
devcall	pipe_read(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer for the bytes		*/
	  int32	count 			/* Maximum bytes to read	*/
	)
{
	int32	retval;			/* Return from pipegetc	*/
	int32	nbytes;			/* Number of bytes found	*/

	if (count <= 0) {
		return SYSERR;
	}

	nbytes = 0;
	while (count > 0) {
		retval = pipe_getc(devptr);
		if ( (retval == SYSERR) || (retval == EOF) ) {
			if (nbytes > 0) {
				/* Some data already read -- return the	*/
				/*  data on this call; the next call	*/
				/*  will return EOF or SYSERR		*/
				return nbytes;
			} else {
				return retval;
			}
		}

		/* Normal case: a data byte has been found */

		*buff++ = 0xff & retval;
		count--;
		nbytes++;
	}
	return nbytes;
}
