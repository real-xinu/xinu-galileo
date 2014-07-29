/* lfibclear.c - lfibclear */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lfibclear  --  clear an in-core copy of an index block
 *------------------------------------------------------------------------
 */
void	lfibclear(
	  struct lfiblk	*ibptr,		/* address of i-block in memory	*/
	  int32		offset		/* file offset for this i-block	*/
	)
{
	int32	i;			/* indexes through array 	*/

	ibptr->ib_offset = offset;	/* assign specified file offset	*/
	for (i=0 ; i<LF_IBLEN ; i++) {	/* clear each data block pointer*/
		ibptr->ib_dba[i] = LF_DNULL;
	}
	ibptr->ib_next = LF_INULL;	/* set next ptr to null		*/
}
