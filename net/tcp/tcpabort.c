/* tcpabort.c - tcpabort */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpabort  -  Close a TCB, releasing waiting processes 
 *------------------------------------------------------------------------
 */
void	tcpabort(
	  struct tcb	*tcbptr		/* Ptr to the TCB		*/
	)
{
	/* Change state to closed */

	tcbptr->tcb_state = TCB_CLOSED;

	/* Release any processes that are waiting to read or write */

	tcpwake (tcbptr, TCPW_READERS|TCPW_WRITERS);

	/* If connection in progress, decrement reference count */

	if (tcbptr->tcb_state > TCB_SYNRCVD)
		tcbunref (tcbptr);

	return;
}
