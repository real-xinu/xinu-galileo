/* tcpdata.c  -  tcpdata */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpdata  -  Handle an incoming segment that contains data
 *------------------------------------------------------------------------
 */
int32	tcpdata(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Pointer to a packet		*/
	)
{
	int32	datalen;		/* Data length			*/
	int32	codelen;		/* Count 1 sequence for FIN	*/
	int32	offset;			/* Offset in segment of new	*/
					/*   data (i.e., data not	*/
					/*   received earlier		*/
	int32	i;			/* counter and index used	*/
					/*   during data copy		*/
	tcpseq	endseq;			/* Ending sequence number after	*/
					/*   new data that arrived	*/
	char	*data;			/* Ptr used during data copy	*/
	char	*wt;			/* Ptr used during data copy	*/

	/* Compute the segment data size */

	datalen = TCP_DATALEN(pkt);
	codelen = offset = 0;

	/* See if FIN has arived */

	if (pkt->net_tcpcode & TCPF_FIN) {
		codelen++;
		tcbptr->tcb_flags |= TCBF_FINSEEN;
		tcbptr->tcb_rfin = pkt->net_tcpseq + datalen;
	}

	/* If no data, we're finished */

	if (datalen == 0 && codelen == 0)
		return 0;

	/* If the reader cannot read this, RST it */

	if (tcbptr->tcb_flags & TCBF_RDDONE && datalen) {
		tcpreset (pkt);
		return SYSERR;
	}

	/* If this segment is too old, drop it */

	if (SEQ_CMP (pkt->net_tcpseq + datalen + codelen,
		     tcbptr->tcb_rbseq) <= 0) {
		tcbptr->tcb_flags |= TCBF_NEEDACK;
		return SYSERR;
	}

	/* If the sender exceeded our window, shame on them */

	if (SEQ_CMP (pkt->net_tcpseq + datalen + codelen,
		     tcbptr->tcb_rbseq + tcbptr->tcb_rbsize) > 0) {
		tcbptr->tcb_flags |= TCBF_NEEDACK;
		return SYSERR;
	}

	/* Discard data already consumed by the reader */

	if (SEQ_CMP (pkt->net_tcpseq, tcbptr->tcb_rbseq) < 0) {
		offset = tcbptr->tcb_rbseq - pkt->net_tcpseq;
		datalen -= offset;
	}
	/* Move to start of new data in segment */

	data = (char *)&pkt->net_tcpsport + TCP_HLEN(pkt) + offset;

	/* Copy data from segment to TCB */

	i = 0;
	wt = tcbptr->tcb_rbdata + pkt->net_tcpseq - tcbptr->tcb_rbseq + offset;
	if (wt >= tcbptr->tcb_rbend)
		wt -= tcbptr->tcb_rbsize;
	while (i < datalen) {
		if (wt >= tcbptr->tcb_rbend)
			wt = tcbptr->tcb_rbuf;
		*wt++ = data[i++];
	}

	/* compute the ending sequence number after the new data */

	endseq = pkt->net_tcpseq + offset + datalen;

	/* See if segment arrived in order */

	if (SEQ_CMP (pkt->net_tcpseq + offset,
				tcbptr->tcb_rbseq + tcbptr->tcb_rblen) <= 0) {
		/* Yes, the data is in order */
		if (endseq - tcbptr->tcb_rbseq >= tcbptr->tcb_rblen) {
			tcbptr->tcb_rblen = endseq - tcbptr->tcb_rbseq;

			tcbptr->tcb_rnext = endseq + codelen;
		}
	} else {
		kprintf("tcpdata: out of order segment\n");
		/* We should deal with out-of-order segments */
	}

	/* See if new data should be pushed to applications */

	if (pkt->net_tcpcode & TCPF_PSH) {
		tcbptr->tcb_flags |= TCBF_RPUSHOK;
		tcbptr->tcb_rpush = endseq;
	}

	/* If data or a FIN arrived, an ACK is needed */

	if (datalen || codelen) {
		tcbptr->tcb_flags |= TCBF_NEEDACK;
		if (tcbptr->tcb_readers > 0) {
			tcbptr->tcb_readers--;
			signal (tcbptr->tcb_rblock);
		}
	}

	/* Return size of data available */

	return datalen + codelen;
}
