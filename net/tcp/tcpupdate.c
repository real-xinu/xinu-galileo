/* tcpupdate.c  -  tcpupdate */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpupdate  -  Update the receive window and other parameters
 *------------------------------------------------------------------------
 */
int32	tcpupdate(
	struct tcb	*tcbptr,		/* Ptr to a TCB			*/
	struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	int32		datalen;	/* Amount of data		*/
	int32		adjust;		/* Adjusted window size		*/
	int32		acklen;		/* Compate ACK to data sent	*/

	/* If starting a connection, nothing to be done */

	if (tcbptr->tcb_state <= TCB_SYNRCVD
	    || !(pkt->net_tcpcode & TCPF_ACK)) {
		return OK;
	}

	/* Check ACK against data sent */

	acklen = SEQ_CMP(pkt->net_tcpack, tcbptr->tcb_suna);
	if (acklen < 0) {
		/* Stale ACK */
		return OK;
	} else if (acklen == 0) {
		/* Potentially duplicate ACK */
		if (TCP_DATALEN(pkt) == 0
		    && pkt->net_tcpwindow == tcbptr->tcb_rwnd)
			tcbptr->tcb_dupacks++;
	} else if (acklen > 0) {
		datalen = min (tcbptr->tcb_sblen,
			       pkt->net_tcpack - tcbptr->tcb_suna);
		tcbptr->tcb_sbdata += datalen;
		tcbptr->tcb_sbdata %= tcbptr->tcb_sbsize;
		tcbptr->tcb_sblen -= datalen;
		tcbptr->tcb_suna = pkt->net_tcpack;
		if (tcbptr->tcb_cwnd < tcbptr->tcb_ssthresh)
			adjust = tcbptr->tcb_mss;
		else
			adjust = tcbptr->tcb_mss
				* tcbptr->tcb_mss
				/ tcbptr->tcb_cwnd;
		tcbptr->tcb_cwnd += adjust ? adjust : 1;
		if (tcbptr->tcb_cwnd > tcbptr->tcb_rwnd)
			tcbptr->tcb_cwnd = tcbptr->tcb_rwnd;
		tcbptr->tcb_dupacks = 0;
		tcbptr->tcb_rtocount = 0;
	}

	/* Update the receive window */

	tcbptr->tcb_rwnd = pkt->net_tcpwindow;

	/* Check for RTT measurement */

	if (tcbptr->tcb_flags & TCBF_RTTPEND
			&& SEQ_CMP(pkt->net_tcpack, tcbptr->tcb_rttseq) >= 0) {
		tcbptr->tcb_flags &= ~TCBF_RTTPEND;
		tcprto (tcbptr);
	}

	/* Correct the RTO timer */

	if (acklen) {
		tcptmdel (tcbptr, TCBC_RTO);
		if (tcbptr->tcb_suna != tcbptr->tcb_snext)
			tcptmset (tcbptr->tcb_rto, tcbptr, TCBC_RTO);
	}

	return OK;
}
