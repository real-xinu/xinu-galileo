/* tcplisten.c  -  tcplisten */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcplisten  -  handle a segment in the LISTEN state
 *------------------------------------------------------------------------
 */
int32	tcplisten(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	struct	tcb	*pnewtcb;	/* Ptr to TCB for new connection*/
	int32	i;			/* Walks through TCP devices	*/

	/* When listening, incoming segment must be a SYN */

	if ((pkt->net_tcpcode & TCPF_SYN) == 0) {
		return SYSERR;
	}

	/* Obtain exclusive use */

	wait (Tcp.tcpmutex);

	/* Find a free TCP pseudo-device for this connection */

	for (i = 0; i < Ntcp; i++) {
		if (tcbtab[i].tcb_state == TCB_FREE)
			break;
	}
	if (i == Ntcp) {

		/* No free devices */

		signal (Tcp.tcpmutex);
		return SYSERR;
	}

	/* Obtain pointer to the device for the new connection	*/
	/*	and initialize the TCB				*/

	pnewtcb = &tcbtab[i];
	tcbclear (pnewtcb);
	pnewtcb->tcb_rbuf = (char *)getmem (65535);
	if (pnewtcb->tcb_rbuf == (char *)SYSERR) {
		signal (Tcp.tcpmutex);
		return SYSERR;
	}
	pnewtcb->tcb_rbsize = 65535;
	pnewtcb->tcb_rbdata = pnewtcb->tcb_rbuf;
	pnewtcb->tcb_rbend = pnewtcb->tcb_rbuf + pnewtcb->tcb_rbsize;
	//pnewtcb->tcb_rbsize = 25*1024;
	pnewtcb->tcb_sbuf = (char *)getmem (65535);
	if (pnewtcb->tcb_sbuf == (char *)SYSERR) {
		freemem ((char *)pnewtcb->tcb_rbuf, 65535);
		signal (Tcp.tcpmutex);
		return SYSERR;
	}
	pnewtcb->tcb_sbsize = 65535;

	/* New connection is in SYN-Received State */

	pnewtcb->tcb_state = TCB_SYNRCVD;
	tcbref (pnewtcb);
	wait (pnewtcb->tcb_mutex);
	if (mqsend (tcbptr->tcb_lq, i) == SYSERR) {
		tcbunref (pnewtcb);
		signal (pnewtcb->tcb_mutex);
		signal (Tcp.tcpmutex);
		return SYSERR;
	}
	signal (Tcp.tcpmutex);

	tcbptr->tcb_qlen++;
	if (tcbptr->tcb_readers > 0) {
		tcbptr->tcb_readers--;
		signal (tcbptr->tcb_rblock);
	}

	/* Copy data from SYN segment to TCB */

	pnewtcb->tcb_lip = pkt->net_ipdst;
	pnewtcb->tcb_lport = pkt->net_tcpdport;
	pnewtcb->tcb_rip = pkt->net_ipsrc;
	pnewtcb->tcb_rport = pkt->net_tcpsport;

	/* Record sequence number and window size */

	pnewtcb->tcb_rnext = pnewtcb->tcb_rbseq = pkt->net_tcpseq + 1;
	pnewtcb->tcb_rwnd = pnewtcb->tcb_ssthresh = pkt->net_tcpwindow;
	pnewtcb->tcb_snext = pnewtcb->tcb_suna = pnewtcb->tcb_ssyn = 1;
	//kprintf("tcplisten: newtcb: rbseq %x, rnext %x\n", pnewtcb->tcb_rbseq, pnewtcb->tcb_rnext);
	/* Handle any data in the segment (unexpected, but required) */

	tcpdata (pnewtcb, pkt);

	/* Can this deadlock? */
	tcpxmit (pnewtcb, pnewtcb->tcb_snext);

	signal (pnewtcb->tcb_mutex);

	return OK;
}
