/* eth_q_read.c - eth_q_read */

#include <xinu.h>

/*------------------------------------------------------------------------
 * eth_q_read - read an incoming packet on Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
devcall	eth_q_read	(
			struct	dentry *devptr,
			void 	*buf,
			uint32	len
			)
{
	struct	ether *ethptr;		/* Ethertab entry pointer	*/
	struct	eth_q_rx_desc *rdescptr;/* Pointer to the descriptor	*/
	struct	netpacket *pktptr;	/* Pointer to packet		*/
	uint32	framelen = 0;		/* Length of the incoming frame	*/
	bool8	valid_addr;
	int32	i;

	ethptr = &ethertab[devptr->dvminor];

	while(1) {

		/* Wait until there is a packet in the receive queue */
		wait(ethptr->isem);

		/* Pointer to the head of the desc list */
		rdescptr = (struct eth_q_rx_desc *)ethptr->rxRing +
							ethptr->rxHead;

		pktptr = (struct netpacket*)rdescptr->buffer1;

		valid_addr = FALSE;

		/* Check if destination address is unicast address */
		if(!memcmp(pktptr->net_ethdst, ethptr->devAddress, 6)) {
			valid_addr = TRUE;
		}
		else if(!memcmp(pktptr->net_ethdst,
                                    NetData.ethbcast,6)) {
            		valid_addr = TRUE;
    		}
		else{
			/* Loop through multicast address array        */
			/* and check if address is a mulitcast address */
			for(i = 0; i < (ethptr->ed_mcc); i++) {
				if(memcmp(pktptr->net_ethdst,
					ethptr->ed_mca[i], 6) == 0){
					valid_addr = TRUE;
					break;
				}
		        }
		}

		if(valid_addr == TRUE){
			/* Get the frame length of the received packet */
			framelen = (rdescptr->status >> 16) & 0x00003FFF;

			/* Adjust the length according to the input parameter */
			if(framelen > len) {
				framelen = len;
			}

			/* Copy the packet into the user provided buffer */
			memcpy(buf, (void*)rdescptr->buffer1, framelen);
		}

        	/* Increment the head of the descriptor list */
		ethptr->rxHead += 1;
		if(ethptr->rxHead >= ETH_QUARK_RX_RING_SIZE) {
			ethptr->rxHead = 0;
		}

		/* Initialize the descriptor with */
        	/* maximum possible frame length  */
		rdescptr->buf1size = sizeof(struct netpacket);

		/* If we are at the end of the ring,      */
        	/* indicate end of ring in the descriptor */
		if(ethptr->rxHead == 0) {
			rdescptr->rdctl1 |= (ETH_QUARK_RDCTL1_RER);
		}

		/* Indicate that the descriptor can be used by the DMA now */
		rdescptr->status = ETH_QUARK_RDST_OWN;

		if(valid_addr == TRUE) {
			break;
		}
	}

	/* Return the no. of bytes copied in the user provided buffer */
	return framelen;

}
