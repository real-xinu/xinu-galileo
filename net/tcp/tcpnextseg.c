/* tcpnextseg.c  -  tcpnextseg */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpnextseg  -  Compute the offset for the next segment
 *------------------------------------------------------------------------
 */
int32	tcpnextseg(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  int32		*offset		/* Offset returned to caller	*/
	)
{
    *offset = tcbptr->tcb_snext - tcbptr->tcb_suna;

	if (tcbptr->tcb_sblen > *offset)
		return (min (tcbptr->tcb_mss, tcbptr->tcb_sblen - *offset));

	return 0;
}
