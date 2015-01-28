/* tcptwait.c  -  tcptwait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcptwait  -  Handle an incoming segment in the TIME-WAIT state
 *------------------------------------------------------------------------
 */
int32	tcptwait(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  struct netpacket *pkt		/* Ptr to a packet		*/
	)
{
	return OK;
}
