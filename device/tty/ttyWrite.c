/* ttyWrite.c - ttyWrite, writcopy */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyWrite - write character(s) to a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyWrite(
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer of characters		*/
	  int32	count 			/* count of character to write	*/
	)
{

	if (count < 0) {
		return SYSERR;
	} else if (count == 0){
		return OK;
	}

	for (; count>0 ; count--) {
		ttyPutc(devptr, *buff++);
	}
	return OK;
}
