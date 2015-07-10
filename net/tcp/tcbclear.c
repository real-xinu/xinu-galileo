/* tcbclear.c - tcbclear */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcbclear  -  clear a TCB entry by resetting all fields
 *------------------------------------------------------------------------
 */
void	tcbclear(
	  struct tcb	*ptcb		/* ptr to the TCB		*/
	)
{
	ptcb->tcb_readers = 0;
	ptcb->tcb_lip = 0;
	ptcb->tcb_rip = 0;
	ptcb->tcb_lport = 0;
	ptcb->tcb_rport = 0;
	ptcb->tcb_flags = 0;
	ptcb->tcb_qlen = 0;
	ptcb->tcb_mss = TCP_ETHMSS; /* Default MSS is Ethernet */
	ptcb->tcb_rnext = 0;
	ptcb->tcb_rwnd = 0;
	ptcb->tcb_rfin = 0;
	ptcb->tcb_rbsize = 0;
	ptcb->tcb_rbdata = NULL;
	ptcb->tcb_rbend = NULL;
	ptcb->tcb_rbseq = 0;
	ptcb->tcb_rblen = 0;
	ptcb->tcb_rbuf = NULL;
	ptcb->tcb_readers = 0;
	ptcb->tcb_suna = 0;
	ptcb->tcb_snext = 0;
	ptcb->tcb_ssyn = 0;
	ptcb->tcb_sfin = 0;
	ptcb->tcb_spush = 0;
	ptcb->tcb_cwnd = ptcb->tcb_mss << 1;
	ptcb->tcb_ssthresh = 0;
	ptcb->tcb_dupacks = 0;
	ptcb->tcb_srtt = 0;
	ptcb->tcb_rttvar = 0;
	ptcb->tcb_rto = 3000;
	ptcb->tcb_rtocount = 0;
	ptcb->tcb_sbsize = 0;
	ptcb->tcb_sbdata = 0;
	ptcb->tcb_sblen = 0;
	ptcb->tcb_sbuf = NULL;
	ptcb->tcb_writers = 0;
}
