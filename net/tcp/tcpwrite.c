/* tcp_send.c  -  tcp_send */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcp_send  -  Write data to a TCP connection
 *------------------------------------------------------------------------
 */
int32	tcp_send(
	int32	slot,	/* Slot in TCB Table		*/
	char	*data,	/* Buffer of data to write	*/
	int32	len	/* Count of chars in the buffer	*/
	)
{
	struct	tcb	*tcbptr;	/* Ptr to TCB for the device	*/
	int32		i;		/* counts bytes of data		*/
	int32		j;		/* used during copy		*/
	int32		curlen;		/* bytes that can be copied	*/

	if((slot < 0) || (slot >= Ntcp)) {
		return SYSERR;
	}

	/* Obtain a pointer to the TCB entry for the device */

	tcbptr = &tcbtab[slot];

	/* Obtain exclusive use of the TCB table */

	wait (Tcp.tcpmutex);
	if (tcbptr->tcb_state <= TCB_LISTEN) {
		/* Connection not yet established */
		signal (Tcp.tcpmutex);
		return SYSERR;
	}

	/* Obtain exclusive use of the specific TCB */

	wait (tcbptr->tcb_mutex);
	signal (Tcp.tcpmutex);

	i = 0;

	/* Loop until all data has been written */

	while (i < len) {
		if (tcbptr->tcb_flags & TCBF_WRDONE)
			break;

		while (tcbptr->tcb_sblen == tcbptr->tcb_sbsize
		       && tcbptr->tcb_state != TCB_CLOSED) {
			tcbptr->tcb_writers++;
			signal (tcbptr->tcb_mutex);
			wait (tcbptr->tcb_wblock);
			wait (tcbptr->tcb_mutex);
		}
		if (tcbptr->tcb_state == TCB_CLOSED) {
			break;
		}

		/* Calculate size of dtaa that can be copied */

		curlen = min (tcbptr->tcb_sbsize - tcbptr->tcb_sblen, len - i);

		/* Copy data */

		for (j = 0; j < curlen; j++) {
			tcbptr->tcb_sbuf[(tcbptr->tcb_sbdata
					+ tcbptr->tcb_sblen
					+ j) % tcbptr->tcb_sbsize] = data[i++];
		}
		tcbptr->tcb_sblen += curlen;
	}

	/* If data was deposited, push it (push on write) */

	if (i > 0) {
		tcbptr->tcb_flags |= TCBF_SPUSHOK;
		tcbptr->tcb_spush = tcbptr->tcb_suna + tcbptr->tcb_sblen;

		tcbref (tcbptr);

		/* send a message to the output process */

		mqsend (Tcp.tcpcmdq, TCBCMD(tcbptr, TCBC_SEND));
	}
	signal (tcbptr->tcb_mutex);

	return i;
}
