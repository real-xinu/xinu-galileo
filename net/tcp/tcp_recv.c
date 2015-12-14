/* tcp_recv.c  -  tcp_recv */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcp_recv  -  Read from a TCP connection
 *------------------------------------------------------------------------
 */
int32	tcp_recv(
		int32	slot,	/* Slot in TCB Table	*/
		char	*data,	/* Ptr to buffer	*/
		int32	len	/* Size of buffer	*/
		)
{
	struct	tcb	*tcbptr;	/* Ptr to a TCB			*/
	int32		i;		/* Iteratives through data	*/
	int32		curlen;		/* Amount of data available	*/
	pid32		child;		/* Process ID of child		*/
	char *		end;		/* addr of the ring buffer until data has to be copied */

	if((slot < 0) || (slot >= Ntcp)) {
		return SYSERR;
	}

	/* Obtain pointer to the TCB for the pseudo device */

	tcbptr = &tcbtab[slot];

	/* Obtain exclusive access to TCB table */

	wait (Tcp.tcpmutex);

	if (tcbptr->tcb_state == TCB_FREE) {
		signal (Tcp.tcpmutex);
		return SYSERR;
	}

	/* Obtain exclusive access to the specific TCB */

	wait (tcbptr->tcb_mutex);
	signal (Tcp.tcpmutex);

	tcbref (tcbptr);

	/* Interpret semantics according to TCB state */

	if (tcbptr->tcb_state == TCB_LISTEN) {

		/* Read on a passive socket acts like accept() */

		while (tcbptr->tcb_qlen == 0
				&& tcbptr->tcb_state != TCB_CLOSED) {
			tcbptr->tcb_readers++;
			signal (tcbptr->tcb_mutex);
			wait (tcbptr->tcb_rblock);
			wait (tcbptr->tcb_mutex);
		}

		/* Recheck state to see why we resumed */

		if (tcbptr->tcb_state == TCB_CLOSED) {
			child = SYSERR;
		} else {
			tcbptr->tcb_qlen--;
			child = mqrecv (tcbptr->tcb_lq);
		}
		tcbunref (tcbptr);
		signal (tcbptr->tcb_mutex);
		if (child == SYSERR){
			return SYSERR;
		} else {
			*(int *)data = child;
		}
		return OK;
	}

	/* Check closed sockets, pushes, etc. etc. */

	i = 0;
	while (i < len) {

		/* Handle read after FIN */

		if ((tcbptr->tcb_flags & TCBF_FINSEEN)
				&& tcbptr->tcb_rbseq == tcbptr->tcb_rfin)
			break;

		if (tcbptr->tcb_flags & TCBF_RDDONE
				&& tcbptr->tcb_rblen == 0)
			break;

		/* A test is needed here: if another reader has already	*/
		/* read the data for which we are waiting or we hit a	*/
		/* FIN, the situation is the same as the above		*/
		/* condition.						*/

		if (tcbptr->tcb_rblen == 0) {
			tcbptr->tcb_readers++;
			signal (tcbptr->tcb_mutex);
			wait (tcbptr->tcb_rblock);
			wait (tcbptr->tcb_mutex);
		}

		/* Compute current data length */

		curlen = min(len - i, tcbptr->tcb_rblen);
		if (tcbptr->tcb_flags & TCBF_RPUSHOK)
			curlen = min (curlen,
					tcbptr->tcb_rpush - tcbptr->tcb_rbseq);

		/* Copy data to caller's buffer */

		end = tcbptr->tcb_rbdata + curlen;
		if (end >= tcbptr->tcb_rbend)
			end -= tcbptr->tcb_rbsize;
		while (tcbptr->tcb_rbdata != end) {
			data[i++] = *tcbptr->tcb_rbdata++;
			if (tcbptr->tcb_rbdata >= tcbptr->tcb_rbend)
				tcbptr->tcb_rbdata = tcbptr->tcb_rbuf;
		}
		tcbptr->tcb_rbseq += curlen;
		tcbptr->tcb_rblen -= curlen;

		/* If data was pushed, reset PUSH flag */

		if (tcbptr->tcb_flags & TCBF_RPUSHOK
				&& tcbptr->tcb_rbseq == tcbptr->tcb_rpush) {
			tcbptr->tcb_flags &= ~TCBF_RPUSHOK;
			break;
		}
	}

	tcbunref (tcbptr);
	signal (tcbptr->tcb_mutex);

	return i;
}
