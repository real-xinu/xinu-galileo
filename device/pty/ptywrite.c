/* ptywrite.c - ptywrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptywrite  -  Write character(s) to a pty device (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ptywrite(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buff,			/* Buffer of characters		*/
	  int32	count 			/* Count of character to write	*/
	)
{
	/* Handle negative and zero counts */
	if (count < 1) {
		return SYSERR;
	} else if (count == 0){
		return OK;
	}


	/* Write count characters one at a time */
	for (; count>0 ; count--) {
		ptyputc(devptr, *buff++);
	}
	return OK;
}
