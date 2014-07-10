/* ethWrite.c -  etherWrite */

#include <xinu.h>

uint32 wr_pktcount = 0;

/*------------------------------------------------------------------------
 * ethWrite - write a packet to an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethWrite (
	struct	dentry	*devptr, 	/* entry in device switch table	*/
	void	*buf,			/* buffer that holds a packet	*/
	uint32	len			/* length of buffer		*/
		)
{
	struct	ether	*ethptr; 	/* ptr to entry in ethertab 	*/
	uint32 rc;

	ethptr = &ethertab[devptr->dvminor];

	/* Verify Ethernet interface is up and arguments are valid */

	if ((ETH_STATE_UP != ethptr->state)
			|| (len < ETH_HDR_LEN)
			|| (len > ETH_MAX_PKT_LEN) ) {
		return SYSERR;
	}	

	rc = ethptr->ethWrite(ethptr, buf, len);
	
	wr_pktcount++;
	//kprintf("WR %d %d\n", wr_pktcount, rc);
	
	return rc;
}
