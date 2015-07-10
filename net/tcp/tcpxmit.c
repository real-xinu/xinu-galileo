/* tcpxmit.c  -  tcpxmit */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpxmit  -  Send a segment if needed
 *------------------------------------------------------------------------
 */
int32	tcpxmit(
	struct tcb	*tcbptr,		/* Ptr to a TCB			*/
	tcpseq		seq				/* seq of the first byte to send */
	)
{
	int32		len;		/* length of segment to send	*/
	int32		offset;		/* Offset of first data byte	*/
	int32		pipe;		/* */
	int32		code;		/* Code bits			*/
	int32		codelen;	/* Number of code bits set	*/
	int32		sent;		/* Has anything been sent?	*/

	sent = 0;
	//kprintf("tcpxmit..\n");
	/* Send as many segments as we can */

	while (1) {

		/* Calculate offset and length for next segment */
		offset = seq - tcbptr->tcb_suna;
		len = tcbptr->tcb_sblen - offset;
		if (tcbptr->tcb_sblen > offset)
			len = min (tcbptr->tcb_mss, len);
		else
			len = 0;

		code = codelen = 0;
		/* The Following handles each code bit */

		/* SYN */

		if (tcbptr->tcb_state <= TCB_SYNRCVD
				&& SEQ_CMP (seq, tcbptr->tcb_ssyn) <= 0) {
			codelen++;
			code |= TCPF_SYN;
		}

		/* FIN */

		if (tcbptr->tcb_flags & TCBF_WRDONE
				&& seq + len == tcbptr->tcb_sfin
				&& SEQ_CMP (tcbptr->tcb_snext, tcbptr->tcb_sfin) <= 0) {
			codelen++;
			code |= TCPF_FIN;
			if (tcbptr->tcb_state == TCB_ESTD
					|| tcbptr->tcb_state == TCB_SYNRCVD)
				tcbptr->tcb_state = TCB_FIN1;
			else if (tcbptr->tcb_state == TCB_CWAIT)
				tcbptr->tcb_state = TCB_LASTACK;
		}

		/* PUSH */

		if (tcbptr->tcb_flags & TCBF_SPUSHOK
				&& SEQ_CMP(seq, tcbptr->tcb_spush) < 0
				&& SEQ_CMP(seq + len, tcbptr->tcb_spush) >= 0) {
			len = min (len, tcbptr->tcb_spush - seq);
			code |= TCPF_PSH;
		}

		if (tcbptr->tcb_state <= TCB_SYNRCVD) {
			pipe = 0;
		} else {
			pipe = seq - tcbptr->tcb_suna;
		}

		/* If we reach this point with no data, check to see	*/
		/* whether an ACK is needed.  If not, simply return	*/
		/* without taking action.  Avoiding action in case of	*/
		/* no data and no ACK allows tcpxmit to be called at	*/
		/* will without requiring a caller to check conditions.	*/

		if ( ( (len + codelen) == 0 )
				|| ( (pipe + len + codelen) > tcbptr->tcb_cwnd ) ) {
			if (sent == 0) {
				tcpack (tcbptr, FALSE);
			}
			return OK;
		}

		/* Send a segment */
		tcpsendseg (tcbptr, offset, len, code);
		seq = seq + len + codelen;

		if (SEQ_CMP(tcbptr->tcb_snext, seq) < 0) {
			tcbptr->tcb_snext = seq;
		}

		sent = 1;
	}

	return OK;
}
