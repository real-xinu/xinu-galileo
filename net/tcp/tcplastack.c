/* tcplastack.c  -  tcplastack */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcplastack  -  Handle the final ACK for the FIN
 *------------------------------------------------------------------------
 */
int32	tcplastack(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	/* See if this ACK covers the FIN */

	if (SEQ_CMP(tcbptr->tcb_suna, tcbptr->tcb_sfin) > 0) {

		/* There must be an RTO pending at this point */

		tcptmdel (tcbptr, TCBC_RTO);

		/* Mark the TCB closed */

		tcbptr->tcb_state = TCB_CLOSED;

		/* Reduce the reference count */

		tcbunref (tcbptr);
	}
	return OK;
}
