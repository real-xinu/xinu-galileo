/* tcprto.c  -  tcprto */

#include <xinu.h>

extern	uint32	ctr1000;
#define TCP_MIN_RTO 100

/*------------------------------------------------------------------------
 *  tcprto  -  Compute round-trip timeout
 *------------------------------------------------------------------------
 */
int32	tcprto(
		struct tcb	*tcbptr		/* Ptr to a TCB			*/
	)
{
	int32		x;		/* Increment			*/
	int32		r;		/* round trip			*/

	r = ctr1000 - tcbptr->tcb_rtttime;
	if (r == 0)
		r = 1;

	/* Use unsigned integer arithmentic for the above calculation to	*/
	/* handle overflow.  We must bound our lower measurement at	*/
	/* 1 millisecond because that's the clock granularity.		*/

	if (tcbptr->tcb_srtt == 0) {

		/* Scaling for powers of two: the smothed round-trip	*/
		/* time, SRTT, is stored as the	actual SRTT times 8,	*/
		/* and RTTVAR is stored as RTTVAR times 4. Because the	*/
		/* following assignment RTTVAR = R/2, we use * 2.	*/

		tcbptr->tcb_srtt = r << 3;
		tcbptr->tcb_rttvar = r << 1;

	} else {
		x = (tcbptr->tcb_srtt >> 3) - r;
		if (x < 0) {
			x = -x;
		}
		tcbptr->tcb_rttvar += -(tcbptr->tcb_rttvar >> 2) + x;
		tcbptr->tcb_srtt += -(tcbptr->tcb_srtt >> 3) + r;
	}
	tcbptr->tcb_rto = (tcbptr->tcb_srtt >> 3) + tcbptr->tcb_rttvar > 1 ? tcbptr->tcb_rttvar : 1;
	if (tcbptr->tcb_rto < TCP_MIN_RTO)
		tcbptr->tcb_rto = TCP_MIN_RTO;

	return tcbptr->tcb_rto;
}
