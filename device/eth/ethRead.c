/* ethRead.c  - ethRead */

#include <xinu.h>

uint32 rd_pktcount = 0;

/*------------------------------------------------------------------------
 * ethRead - read a packet from an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethRead (
	struct	dentry	*devptr,	/* entry in device switch table	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
		)
{
	struct	ether	*ethptr;	/* ptr to entry in ethertab	*/
	uint32 rc;

	ethptr = &ethertab[devptr->dvminor];

	/* Verify Ethernet interface is up and arguments are valid */

	if ((ETH_STATE_UP != ethptr->state)
			|| (len < ETH_HDR_LEN)) {
		return SYSERR;
	}

	rc = ethptr->ethRead(ethptr, buf, len);
	
	rd_pktcount++;
	//kprintf("RD %d %d\n", rd_pktcount, rc);
	
	return rc;
}
