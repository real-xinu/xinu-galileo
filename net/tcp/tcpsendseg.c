/* tcpsendseg.c  -  tcpsendseg */

#include <xinu.h>

extern	uint32	ctr1000;

/*------------------------------------------------------------------------
 *  tcpsendseg  -  Send a TCP segment for a specified TCB
 *------------------------------------------------------------------------
 */
void	tcpsendseg(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  int32		offset,		/* Offset for data to send	*/
	  int32		len,		/* Length of TCP data		*/
	  int32		code		/* Code value to use		*/
	)
{
	int		i;		/* counts data bytes during copy*/
	char		*data;		/* ptr to data being copied	*/

	struct netpacket *pkt;		/* ptr for new formant		*/

	/* Allocate a network buffer */

	pkt = tcpalloc (tcbptr, len);

	if ((int32)pkt == SYSERR) {
		return;
	}

	pkt->net_tcpseq = tcbptr->tcb_suna + offset;
	pkt->net_tcpcode |= code;
	if(pkt->net_tcpcode & TCPF_SYN) {
		pkt->net_tcpcode += 0x1000;
		data = (char *)pkt->net_tcpdata;
		data[0] = 2;
		data[1] = 4;
		*((uint16 *)&data[2]) = htons(1500-50);
		pkt->net_iplen += 4;
	}

	//kprintf("tcpsendseg: tcpcode %x\n", pkt->net_tcpcode);

	data = ((char *)&pkt->net_tcpsport + TCP_HLEN(pkt));
	for (i = 0; i < len; i++) {
		data[i] = tcbptr->tcb_sbuf[(tcbptr->tcb_sbdata + offset + i)
			% tcbptr->tcb_sbsize];
	}

	if (tcbptr->tcb_suna == tcbptr->tcb_snext) {/* No outstanding data */
		tcptmset (tcbptr->tcb_rto, tcbptr, TCBC_RTO);
	}

	if (tcbptr->tcb_flags & TCBF_ACKPEND) {
		tcptmdel (tcbptr, TCBC_DELACK);
	}
	tcbptr->tcb_flags &= ~(TCBF_NEEDACK | TCBF_ACKPEND);

	if (!(tcbptr->tcb_flags & TCBF_RTTPEND) && len) {
		tcbptr->tcb_flags |= TCBF_RTTPEND;
		tcbptr->tcb_rttseq = pkt->net_tcpseq;
		tcbptr->tcb_rtttime = (int)ctr1000;
	}
	//kprintf("OUT: seq %x ackseq %x\n", pkt->net_tcpseq, pkt->net_tcpack);
	//pdumph(pkt);
	//kprintf("calling ip_send\n");
	//ip_send (pkt);
	ip_enqueue(pkt);
	return;
}
