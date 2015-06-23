/* tcpdisp.c  -  tcpnull, tcpdisp */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpnull -  Do nothing (entry in tcpstatesw when no action required)
 *------------------------------------------------------------------------
 */
int32	tcpnull(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to IP (not used)		*/
	)
{
	return OK;
}

/*extern int32 *tcplisten(struct tcb *, struct netpacket *);
extern int32 *tcpsynsent(struct tcb *, struct netpacket *);
extern int32 *tcpsynrcvd(struct tcb *, struct netpacket *);
extern int32 *tcpestd(struct tcb *, struct netpacket *);
extern int32 *tcpfin1(struct tcb *, struct netpacket *);
extern int32 *tcpfin2(struct tcb *, struct netpacket *);
extern int32 *tcpcwait(struct tcb *, struct netpacket *);
extern int32 *tcpclosing(struct tcb *, struct netpacket *);
extern int32 *tcplastack(struct tcb *, struct netpacket *);
extern int32 *tcptwait(struct tcb *, struct netpacket *);*/
int32 (*tcpstatesw[]) (struct tcb *tcbptr,struct netpacket *pkt)
  = {
	tcpnull,			/* CLOSED			*/
	tcplisten,			/* LISTEN			*/
	tcpsynsent,			/* SYN SENT			*/
	tcpsynrcvd,			/* SYN RCVD			*/
	tcpestd,			/* ESTABLISHED 			*/
	tcpfin1,			/* FIN WAIT-1			*/
	tcpfin2,			/* FIN WAIT-2			*/
	tcpcwait,			/* CLOSE WAIT			*/
	tcpclosing,			/* CLOSING			*/
	tcplastack,			/* LAST ACK			*/
	tcptwait,			/* TIME WAIT			*/
};


/*------------------------------------------------------------------------
 *  tcpdisp  -  Use the current state of the TCB to dispatch segment
 *			processing to one of the input functions
 *------------------------------------------------------------------------
 */
void	tcpdisp(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Pointer to packet		*/
	)
{
	int32		state;		/* state of the TCB		*/
/*DEBUG*///char *dnames[] = {"tcpnull","tcplisten","tcpsynsent","tcpsynrcvd",
/*DEBUG*///	"tcpestd","tcpfin1","tcpfin2","tcpcwait","tcpclosing",
/*DEBUG*///	"tcplastack","tcptwait"};

	/* Obtain the state from the TCB */

	state = tcbptr->tcb_state;


	/* Handle a reset */

	if (pkt->net_tcpcode & TCPF_RST) {
		if (state == TCB_LISTEN)
			return;
		else if (state == TCB_SYNSENT
			 && pkt->net_tcpack == tcbptr->tcb_snext)
			tcpabort (tcbptr);
		else if (pkt->net_tcpseq >= tcbptr->tcb_rnext
		    && pkt->net_tcpseq <= tcbptr->tcb_rnext + tcbptr->tcb_rwnd)
			tcpabort (tcbptr);
		return;
	}

	/* Handle an incoming ACK */

	if ((pkt->net_tcpcode & TCPF_ACK) &&
	    (pkt->net_tcpack < tcbptr->tcb_suna
	    || pkt->net_tcpack > tcbptr->tcb_snext)) {
		if (state <= TCB_SYNRCVD) {
			tcpreset (pkt);
		} else {
			tcpack (tcbptr, TRUE);
		}
		return;
	}

	/* Handle SYN */

	if (pkt->net_tcpcode & TCPF_SYN && state > TCB_SYNRCVD) {

		/* Is this the _right_ SYN? */

		pkt->net_tcpcode &= ~TCPF_SYN;
		pkt->net_tcpseq++;
	}

	tcpupdate (tcbptr, pkt);

	/* Dispatch processing according to the state of the TCB */

/*DEBUG*/ //kprintf("tcpdisp: dispatching to %s\n",dnames[state]);
	(tcpstatesw[state]) (tcbptr, pkt);

	return;
}
