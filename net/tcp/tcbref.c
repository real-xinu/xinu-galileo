/* tcbref.c - tcbref, tcbunref */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcbref  -  increment the reference count of a TCB
 *------------------------------------------------------------------------
 */
void	tcbref(
	  struct tcb	*ptcb		/* Ptr to a TCB			*/
       )
{
	ptcb->tcb_ref++;
	return;
}

/*------------------------------------------------------------------------
 *  tcbunref  -  decrement the reference count of a TCB and free the
 *			TCB if the reference count reaches zero
 *------------------------------------------------------------------------
 */
void	tcbunref(
	  struct tcb	*ptcb		/* Ptr to a TCB			*/
       )
{
	if (--ptcb->tcb_ref <= 0) {
		freemem ((char *)ptcb->tcb_rbuf, ptcb->tcb_rbsize);
		freemem ((char *)ptcb->tcb_sbuf, ptcb->tcb_sbsize);
		ptcb->tcb_state = TCB_FREE;
	}
	return;
}
