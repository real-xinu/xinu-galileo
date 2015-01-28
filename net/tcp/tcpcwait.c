/* tcpcwait.c  -  tcpcwait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpcwait  -  perform null action when in wait state
 *------------------------------------------------------------------------
 */
int32	tcpcwait(
	  struct tcb	*tcbptr,		/* Ptr to a TCB		*/
	  struct netpacket *pkt			/* Ptr to a packet	*/
	)
{
	return OK;
}
