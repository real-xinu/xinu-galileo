/* tcpsynrcvd.c  -  tcpsynrcvd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpsynrcvd  -  Handle an incoming segment in state SYN-RECEIVED
 *------------------------------------------------------------------------
 */
int32	tcpsynrcvd(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	/* Check for ACK or bas sequence number */

	if (!(pkt->net_tcpcode & TCPF_ACK)
	    || pkt->net_tcpseq != tcbptr->tcb_rnext) {
	    	//kprintf("tcpsynrcvd: no ack or bad seq\n");
		return SYSERR;
	}

	/* The following increment of the refernce count is for	*/
	/*    the system because the TCB is now synchronized	*/

	tcbref (tcbptr);

	/* Change state to ESTABLISHED */

	tcbptr->tcb_state = TCB_ESTD;

	/* Move past SYN */

	tcbptr->tcb_suna++;
	if (tcpdata (tcbptr, pkt)) {
		tcpxmit (tcbptr, tcbptr->tcb_snext);
	}

	/* Permit reading */

	if (tcbptr->tcb_readers > 0) {
		tcbptr->tcb_readers--;
		signal (tcbptr->tcb_rblock);
	}

	return OK;
}
