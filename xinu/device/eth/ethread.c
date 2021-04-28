/* ethread.c - ethread */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethread  -  Read an incoming packet on Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
devcall	ethread	(
	  struct dentry	*devptr,	/* Entry in device switch table	*/
	  char	*buf,			/* Buffer for the packet	*/
	  int32	len 			/* Size of the buffer		*/
	)
{
	struct	ethcblk *ethptr;	/* Ethertab entry pointer	*/
	struct	eth_q_rx_desc *rdescptr;/* Pointer to the descriptor	*/
	struct	netpacket *pktptr;	/* Pointer to packet		*/
	int32	seglen = 0;		/* Length of current segment	*/
	int32	framelen = 0;		/* Length of the incoming frame	*/
	int32	copylen = 0;		/* Length of data to be copied	*/
	char	*cptr;			/* Address of where to copy data*/
	bool8	first, last;		/* First, last segment flag	*/
	bool8	valid_addr;
	int32	i;

	ethptr = &ethertab[devptr->dvminor];

	pktptr = (struct netpacket *)buf;

	while(1) {

		/* Wait until there is a packet in the receive queue */

		wait(ethptr->isem);

		/* Point to the head of the descriptor list */

		rdescptr = (struct eth_q_rx_desc *)ethptr->rxRing +
							ethptr->rxHead;

		/* Check if this is the first and/or last segment */

		first = (rdescptr->status & ETH_QUARK_RDST_FS) != 0;
		last  = (rdescptr->status & ETH_QUARK_RDST_LS) != 0;

		if(first) { /* This is the first segment */

			/* Initalize the copy pointer and all counters */

			cptr = buf;
			seglen = framelen = copylen = 0;
		}

		/* Extract the segment length from descriotor */

		seglen = (rdescptr->status >> 16) & 0x3FFF;

		if(last) { /* Last segment */

			/* The segment length is the total, length	*/
			/* Calculate current segment length		*/

			seglen -= framelen;
		}

		/* Update the total length */

		framelen += seglen;

		/* Compute the amount we need to copy */

		copylen = seglen;
		if(framelen > len) {
			copylen -= (framelen - len);
		}

		/* Copy the data if we can */

		if(copylen > 0) {
			memcpy(cptr, (void *)rdescptr->buffer1,
							(uint32)copylen);
			cptr += copylen;
		}

		if(last) { /* Last segment, see if we can accept it */
			
			/* See if destination address is our unicast  */

			if(!memcmp(pktptr->net_ethdst,
						ethptr->devAddress, 6)) {
				valid_addr = TRUE;

			/* See if destination address is broadcast */

			} else if(!memcmp(pktptr->net_ethdst,
					    NetData.ethbcast,6)) {
				valid_addr = TRUE;

			/* Multicast, See if we should accept it */

			} else {
				valid_addr = FALSE;
				for(i = 0; i < (ethptr->ed_mcc); i++) {
					if(memcmp(pktptr->net_ethdst,
					   ethptr->ed_mca[i], 6) == 0){
						valid_addr = TRUE;
						break;
					}
				}
			}
		}

        	/* Increment the head of the descriptor list */

		ethptr->rxHead += 1;
		if(ethptr->rxHead >= ETH_QUARK_RX_RING_SIZE) {
			ethptr->rxHead = 0;
		}

		/* Reset the descriptor to max possible frame len */

		rdescptr->buf1size = ETH_BUF_SIZE;

		/* If we reach the end of the ring, mark the descriptor	*/

		if(ethptr->rxHead == 0) {
			rdescptr->rdctl1 |= (ETH_QUARK_RDCTL1_RER);
		}

		/* Indicate that the descriptor is ready for DMA input */

		rdescptr->status = ETH_QUARK_RDST_OWN;

		if(valid_addr == TRUE) {
			break;
		}
	}

	/* Return the number of bytes returned from the packet */

	return (framelen > len) ? len : framelen;

}
