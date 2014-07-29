/* lflWrite.c  -  lfWrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflWrite  --  write data to a previously opened local disk file
 *------------------------------------------------------------------------
 */
devcall	lflWrite (
	  struct dentry *devptr,	/* entry in device switch table */
	  char	*buff,			/* buffer holding data to write	*/
	  int32	count			/* number of bytes to write	*/
	)
{
	int32		i;		/* number of bytes written	*/

	if (count < 0) {
		return SYSERR;
	}
	for (i=0; i<count; i++) {
		if (lflPutc(devptr, *buff++) == SYSERR) {
			return SYSERR;
		}
	}
	return count;
}
