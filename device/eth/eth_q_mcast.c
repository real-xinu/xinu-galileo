/* eth_q_mcast.c - eth_q_add_mcast_addr, eth_q_remove_mcast_addr */

#include <xinu.h>

/*--------------------------------------------------------------------------
 * eth_q_add_mcast -  add multicast address to Intel Quark Ethernet
 *--------------------------------------------------------------------------
 */
int32	eth_q_add_mcast	(
			struct	ether *ethptr, 		/* Pointer to control block */
	  		byte	addr[ETH_ADDR_LEN]	/* Mutlicast address to be added */
			) 
{
	int16	mcast_count;

	struct eth_q_csreg *csrptr = (struct eth_q_csreg *)ethptr->csr;
        
	/*Set the Pass all multicast bit in MAC Frame Filter Register */
	csrptr->macff |= 0x00000010;

	/* Get number of multicast addresses in array */
	mcast_count = ethptr->ed_mcc;

	/*Copy address to last location in array */
	/*      as long as limit is not exceeded */
	if(mcast_count < ETH_NUM_MCAST){
		memcpy(ethptr->ed_mca[mcast_count],addr,ETH_ADDR_LEN); 	
		mcast_count++;
		ethptr->ed_mcc = mcast_count;
		return OK;
	}
	else {
		return SYSERR;
	}
}

/*------------------------------------------------------------------------
 * eth_q_remove_mcast - remove a multicast address from Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
int32	eth_q_remove_mcast	(
				struct	ether *ethptr,/* Pointer to control block */
				byte	addr[ETH_ADDR_LEN]/* Mutlicast adddress to be removed     */
				)
{
	int16	mcast_count;
	int32	i, j;

	mcast_count = ethptr->ed_mcc;

	/* Loop through array to find the multicast address array to */
	/*      find index of address to be removed                  */
	for(i = 0; i < mcast_count; i++) {
		if(!memcmp(addr, ethptr->ed_mca[i], ETH_ADDR_LEN)) {

			/* Shift the values of the array up */
			for(j = i; j < mcast_count; j++) {
				memcpy(ethptr->ed_mca[j],
					ethptr->ed_mca[j+1],ETH_ADDR_LEN);
				memset(ethptr->ed_mca[j+1],'0',
					ETH_ADDR_LEN);
			}
			break;
		}
	}

	if(i < mcast_count) { /* Mcast address found and removed */

		/* Decrement the number of multicast addresses */
		mcast_count--;
		ethptr->ed_mcc = mcast_count;
		return OK;
	}
	else { /* Mcast address not found */
		return SYSERR;
	}
}
