/* tcpestd.c  -  tcpestd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpestd  -  Handle an arriving segment for the Established state
 *------------------------------------------------------------------------
 */
int32	tcpestd(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	/* First, process data in the segment */

	tcpdata (tcbptr, pkt);

	/* If a FIN has been seen, move to Close-Wait */

	//kprintf("tcpestd: %d rnext %x rfin %x\n", tcbptr->tcb_flags&TCBF_FINSEEN, tcbptr->tcb_rnext, tcbptr->tcb_rfin);
	if (tcbptr->tcb_flags & TCBF_FINSEEN
	    && SEQ_CMP(tcbptr->tcb_rnext, tcbptr->tcb_rfin) > 0) {
	    	//kprintf("state changed to CWAIT\n");
		tcbptr->tcb_state = TCB_CWAIT;
	}

	/* Transmit a response immediately */

	tcpxmit (tcbptr, tcbptr->tcb_snext);

	return OK;
}
