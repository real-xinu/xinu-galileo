/* lfibget.c - lfibget */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lfibget  --  get an index block from disk given its number (assumes
 *			mutex is held)
 *------------------------------------------------------------------------
 */
void	lfibget(
	  did32		diskdev,	/* device ID of disk to use	*/
	  ibid32	inum,		/* ID of index block to fetch	*/
	  struct lfiblk	*ibuff		/* buffer to hold index block	*/
	)	
{
	char	*from, *to;		/* pointers used in copying	*/
	int32	i;			/* loop index used during copy	*/
	char	dbuff[LF_BLKSIZ];	/* ibuff to hold disk block	*/

	/* Read disk block that contains the specified index block */

	read(diskdev, dbuff, ib2sect(inum));

	/* Copy specified index block to caller's ibuff */

	from = dbuff + ib2disp(inum);
	to = (char *)ibuff;
	for (i=0 ; i<sizeof(struct lfiblk) ; i++)
		*to++ = *from++;
	return;
}
