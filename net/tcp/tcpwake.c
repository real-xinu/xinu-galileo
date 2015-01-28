
/* tcpwake.c  -  tcpwake */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpwake  -  Awaken up readers, writers, or both and return count
 *------------------------------------------------------------------------
 */
int32	tcpwake(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  int32		cond		/* Condition			*/
	)
{
	int32	awakened = 0;

	/* Check condition for readers and awaken */

	if (cond & TCPW_READERS) {
		awakened += tcbptr->tcb_readers;
		while (tcbptr->tcb_readers--)
			signal (tcbptr->tcb_rblock);
	}

	/* Check condition for writers and awaken */

	if (cond & TCPW_WRITERS) {
		awakened += tcbptr->tcb_writers;
		while (tcbptr->tcb_writers--)
			signal (tcbptr->tcb_wblock);
	}

	/* Return count */

	return awakened;
}
