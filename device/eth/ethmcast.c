/* ethmcast.c - ethmcast_add, ethmcast_remove */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethmcast_add  -  Add multicast address to Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
int32	ethmcast_add	(
	  struct ethcblk *ethptr, 		/* Ptr to control block	*/
	  byte	 addr[ETH_ADDR_LEN]		/* Mcast addr to add	*/
	)
{
	int16	mcast_count;

	struct eth_q_csreg *csrptr = (struct eth_q_csreg *)ethptr->csr;

	/*Set the Pass all multicast bit in MAC Frame Filter Register */

	csrptr->macff |= 0x00000010;

	/* Get number of multicast addresses in array */

	mcast_count = ethptr->ed_mcc;

	/* Add address to array, provided limit is not exceeded */

	if(mcast_count < ETH_NUM_MCAST){
		memcpy(ethptr->ed_mca[mcast_count],addr,ETH_ADDR_LEN); 	
		mcast_count++;
		ethptr->ed_mcc = mcast_count;
		return OK;
	} else {
		return SYSERR;
	}
}

/*------------------------------------------------------------------------
 * ethmcast_remove  -  Remove multicast addr. from Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
int32	ethmcast_remove	(
	  struct ethcblk *ethptr,	/* Pointer to control block	*/
	  byte	 addr[ETH_ADDR_LEN]	/* Mcast address to remove	*/
	)
{
	int16	mcast_count;
	int32	i, j;

	mcast_count = ethptr->ed_mcc;

	/* Find multicast address in the array */

	for (i = 0; i < mcast_count; i++) {
		if (memcmp(addr, ethptr->ed_mca[i], ETH_ADDR_LEN) ==0) {

			/* Shift values to fill in the hole */

			for(j = i; j < mcast_count; j++) {
				memcpy(ethptr->ed_mca[j],
					ethptr->ed_mca[j+1],ETH_ADDR_LEN);
				memset(ethptr->ed_mca[j+1],'0',
					ETH_ADDR_LEN);
			}
			break;
		}
	}

	if(i < mcast_count) {	/* Mcast address was removed */

		/* Decrement the number of multicast addresses */

		mcast_count--;
		ethptr->ed_mcc = mcast_count;
		return OK;
	} else {		/* Mcast address was not found */
		return SYSERR;
	}
}
