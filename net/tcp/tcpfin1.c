/* tcpfin1.c  -  tcpfin1 */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpfin1  -  Handle an incoming segment in the FIN1 state
 *------------------------------------------------------------------------
 */
int32	tcpfin1(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	int32	rfin;			/* Did we receive a FIN?	*/
	int32	ack;			/* Did we send an ACK		*/

	rfin = ack = 0;

	/* Handle data in the segment first */

	tcpdata (tcbptr, pkt);

	/* Check whether FIN has been receives and ACK sent */

	if (tcbptr->tcb_flags & TCBF_FINSEEN &&
	    SEQ_CMP(tcbptr->tcb_rnext, tcbptr->tcb_rfin) > 0)
		rfin = 1;

	if (SEQ_CMP(tcbptr->tcb_suna, tcbptr->tcb_sfin) > 0)
		ack = 1;

	if (rfin && ack) {

		/* FIN was received and ours was ACKed, so the	*/
		/*     connection is closed and TCB can expire	*/

		tcptmset (TCP_MSL << 1, tcbptr, TCBC_EXPIRE);
		tcbptr->tcb_state = TCB_TWAIT;
	} else if (rfin)

		/* FIN was received - connection is closing */

		tcbptr->tcb_state = TCB_CLOSING;
	else if (ack)

		/* ACK arrived and ew move to FIN-WAIT2 */
		tcbptr->tcb_state = TCB_FIN2;

	tcpxmit (tcbptr, tcbptr->tcb_snext);

	return OK;
}
