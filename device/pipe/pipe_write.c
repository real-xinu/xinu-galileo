/* pipe_write.c - pipe_write */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  pipe_write  -  write one or more bytes of data to a pipe device
 *------------------------------------------------------------------------
 */
devcall	pipe_write(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer for the bytes		*/
	  int32	count 			/* Count of bytes to write	*/
	)
{
	int32	nbytes;			/* Count of bytes written	*/
	int32	retval;			/* Return value from pipe_putc	*/

	/* Ensure count is valid */

	if (count <= 0) {
		return SYSERR;
	}

	nbytes = 0;
	while(count > 0) {
		retval = pipe_putc(devptr, *buff++);
		if (retval == SYSERR) {
			if (nbytes > 0) {
				/* Report what has been written on this	*/
				/*  call and SYSERR on next call	*/
				return nbytes;
			} else {
				return SYSERR;
			}
		}
		nbytes++;
		count--;
	}
	return nbytes;
}
