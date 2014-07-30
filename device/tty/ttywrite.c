/* ttyWrite.c - ttyWrite, writcopy */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyWrite  -  Write character(s) to a tty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ttyWrite(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of characters		*/
	  int32	count 			/* Count of character to write	*/
	)
{
	/* Handle negative and zero counts */

	if (count < 0) {
		return SYSERR;
	} else if (count == 0){
		return OK;
	}

	/* Write count characters one at a time */

	for (; count>0 ; count--) {
		ttyPutc(devptr, *buff++);
	}
	return OK;
}
