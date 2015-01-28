/* tcprto.c  -  tcprto */

#include <xinu.h>

extern	uint32	ctr1000;

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

	r = min ((int)ctr1000 - tcbptr->tcb_rtttime, 1);

	/* Use signed integer arithmentic for the above calculation to	*/
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
		tcbptr->tcb_rttvar += -(tcbptr->tcb_rttvar << 2) + x;
		tcbptr->tcb_srtt += -(tcbptr->tcb_srtt >> 3) + r;
	}
	tcbptr->tcb_rto = (tcbptr->tcb_srtt >> 3) + tcbptr->tcb_rttvar;

	return tcbptr->tcb_rto;
}
