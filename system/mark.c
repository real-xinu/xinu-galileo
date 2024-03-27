/* mark.c - markinit, mark */

#include <xinu.h>

int32	*marks[MAXMARK];		/* Pointers to marked locations	*/
int32	nmarks;				/* Number of marked locations	*/
sid32	mkmutex;			/* Mutual exclusion semaphore	*/

/*------------------------------------------------------------------------
 *  markinit  -  Called once at system startup
 *------------------------------------------------------------------------
 */
void	markinit(void)
{
	nmarks = 0;
	mkmutex = semcreate(1);
}


/*------------------------------------------------------------------------
 *  notmarked  -  Return nonzero if a location has not been marked
 *------------------------------------------------------------------------
 */
syscall	notmarked(memmark loc)
{	
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();

	/* See if the location has been marked */

	if (loc[0]<0 || loc[0]>=nmarks || marks[loc[0]] != loc) {
		restore(mask);
		return FALSE;
	}
	return TRUE;
}


/*------------------------------------------------------------------------
 *  mark  -  Mark a specified memory location
 *------------------------------------------------------------------------
 */
syscall	mark(
	  int32	*loc			/* Location to mark		*/
	)

{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();

	/* If location is already marked, do nothing */

	if ( (*loc>=0) && (*loc<nmarks) && (marks[*loc]==loc) ) {
		restore(mask);
		return OK;
	}

	/* If no more memory marks are available, indicate an error */

	if (nmarks >= MAXMARK) {
		restore(mask);
		return SYSERR;
	}

	/* Obtain exclusive access and mark the specified location */

	marks[ (*loc) = nmarks++ ] = loc;
	restore(mask);
	return OK;
}
