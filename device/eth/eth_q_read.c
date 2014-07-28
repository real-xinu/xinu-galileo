/* quark_eth_read.c - quark_eth_read */

#include <xinu.h>

devcall	quark_eth_read(
                struct ether *ethptr,
                void *buf,
                uint32 len
        )
{

	struct	quark_eth_rx_desc *rdescptr;	/* Pointer to the descriptor	*/
	uint32	framelen;			/* Length of the incoming frame	*/

	/* Wait until there is a packet in the receive queue */
	wait(ethptr->isem);

	/* Pointer to the head of the desc list */
	rdescptr = (struct quark_eth_rx_desc *)
			((struct quark_eth_rx_desc *)ethptr->rxRing + ethptr->rxHead);


	int valid_addr = FALSE;
	struct netpacket *netpktptr = (struct netpacket*)rdescptr->buffer1;

	/* Check if destination address is unicast address */
	int compare;
	compare = memcmp(netpktptr->net_ethdst, ethptr->devAddress, 6);
	if(compare == 0){
		valid_addr = TRUE;
	}
	else{
		/* Check if destination address is broadcast address*/
		compare = memcmp(netpktptr->net_ethdst,
                                    NetData.ethbcast,6);
		if(compare == 0){
            valid_addr = TRUE;
    }
		else{
		        int i;
            /* Loop through multicast address array        */
            /* and check if address is a mulitcast address */
		        for(i=0;i < (ethptr->ed_mcc);i++){
			        if(memcmp(netpktptr->net_ethdst,
                                     ethptr->ed_mca[i], 6) == 0){
				        valid_addr = TRUE;
				        break;
			        }
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
	else{
		framelen = 0;
	}

        /* Increment the head of the descriptor list */
	ethptr->rxHead += 1;
	if(ethptr->rxHead >= QUARK_ETH_RX_RING_SIZE) {
		ethptr->rxHead = 0;
	}

	/* Initialize the descriptor with */
        /* maximum possible frame length  */
	rdescptr->buf1size = sizeof(struct netpacket);

	/* If we are at the end of the ring,      */
        /* indicate end of ring in the descriptor */
	if(ethptr->rxHead == 0) {
		rdescptr->rdctl1 |= (QUARK_ETH_RDCTL1_RER);
	}

	/* Indicate that the descriptor can be used by the DMA now */
	rdescptr->status = QUARK_ETH_RDST_OWN;

	/* Return the no. of bytes copied in the user provided buffer */
	return framelen;

}
