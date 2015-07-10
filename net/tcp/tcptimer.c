/* tcptimer.c  -  tcptmset, tcptmdel */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcptmset  -  Set a timed TCP event
 *------------------------------------------------------------------------
 */
void	tcptmset(
	  int32		delay,		/* Delay in milliseconds	*/
	  struct tcb	*tcbptr,	/* ptr to TCB for the event	*/
	  int32		message		/* Event message		*/
	)
{
	/* Increment reference count for TCB */
	tcbref (tcbptr);

	/* Set the event */

	tmset (delay, Tcp.tcpcmdq, TCBCMD(tcbptr, message));
	return;
}

/*------------------------------------------------------------------------
 *  tcptmdel  -  Delete a timed TCP event
 *------------------------------------------------------------------------
 */
void	tcptmdel(
	  struct tcb	*tcbptr,	/* ptr to TCB for the event	*/
	  int32		message		/* Event to delete		*/
	)
{
	/* Delete the event */
	if (tmdel (Tcp.tcpcmdq, TCBCMD(tcbptr, message)) == OK) {

		/* Decrement the refernce count */

		tcbunref (tcbptr);
	}
	return;
}
