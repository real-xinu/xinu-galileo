/* tcpack.c  -  tcpack */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpack  -  Send or schedule transmission of a TCP Acknowledgement
 *------------------------------------------------------------------------
 */
void	tcpack(
	  struct tcb	*tcbptr,	/* Ptr to the TCB		*/
	  int32		force		/* Should we force transmission?*/
	)
{
	struct netpacket *pkt;	/* Pointer to a datagram	*/

	/* If not forcing transmission, schedule an event */

	if (!force) {
		if (!(tcbptr->tcb_flags & TCBF_NEEDACK)) {
			//kprintf("tcpack: no need\n");
			return;
		}
		if (!(tcbptr->tcb_flags & TCBF_ACKPEND)) {
			tcbptr->tcb_flags |= TCBF_ACKPEND;
			tcptmset (TCP_ACKDELAY, tcbptr, TCBC_DELACK);
			return;
		}
	}

	/* If we reach this point, an ACK is pending, or the caller	*/
	/*   has specified forcing an ACK, so go send it now		*/

	if (tcbptr->tcb_flags & TCBF_ACKPEND) {
		tcptmdel (tcbptr, TCBC_DELACK);
	}

	/* Allocate a datagram buffer and fill in IP header */

	pkt = tcpalloc(tcbptr, 0);

	/* Fill in TCP header */

	pkt->net_tcpseq = tcbptr->tcb_snext;

	tcbptr->tcb_flags &= ~(TCBF_NEEDACK | TCBF_ACKPEND);

	//kprintf("OUT: seq %x ackseq %x\n", pkt->net_tcpseq, pkt->net_tcpack);
	//pdumph(pkt);
	ip_enqueue(pkt);
	return;
}
