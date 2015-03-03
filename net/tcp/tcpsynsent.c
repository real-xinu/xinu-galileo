/* tcpsynsent.c  -  tcpsynsent */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpsynsent  -  Handle an incoming segment in the SYN-SENT state
 *------------------------------------------------------------------------
 */
int32	tcpsynsent(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	/* Must receive a SYN or reset connection */

	if (!(pkt->net_tcpcode & TCPF_SYN)) {
		tcpreset (pkt);
		return SYSERR;
	}

	if((!(pkt->net_tcpcode & TCPF_ACK)) ||
	   (pkt->net_tcpack != tcbptr->tcb_snext)) {
	   	return SYSERR;
	}

	/* Move past the SYN */

	tcbptr->tcb_suna++;

	/* Move to ESTABLISHED state */

	tcbptr->tcb_state = TCB_ESTD;

	/* Set up parameters, such as the window size */

	tcbptr->tcb_rnext = tcbptr->tcb_rbseq = ++pkt->net_tcpseq;
	tcbptr->tcb_rwnd = tcbptr->tcb_ssthresh = pkt->net_tcpwindow;
	pkt->net_tcpcode &= ~TCPF_SYN;

	/* Send an ACK */

	tcpdata (tcbptr, pkt);
	tcpack (tcbptr, TRUE);

	if(tcbptr->tcb_readers) {
		tcbptr->tcb_readers--;
		signal(tcbptr->tcb_rblock);
	}

	return OK;
}
