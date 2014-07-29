/* eth_q_write.c - eth_q_write */

#include <xinu.h>

/*------------------------------------------------------------------------
 * eth_q_write - enqueue a packet for transmission on Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
devcall	eth_q_write	(
			struct	dentry *devptr,
			void	*buf,
			uint32	len
			)
{
	struct	ether *ethptr;
	struct	eth_q_csreg *csrptr;
	volatile struct	eth_q_tx_desc *descptr;
	uint32 i;

	ethptr = &ethertab[devptr->dvminor];

	csrptr = (struct eth_q_csreg *)ethptr->csr;

	/* Wait for an empty slot in the transmit desc ring */
	wait(ethptr->osem);

	/* Pointer to the tail of the desc ring */
	descptr = (struct eth_q_tx_desc *)ethptr->txRing + ethptr->txTail;

	/* Increment the tail index */
	ethptr->txTail += 1;
	if(ethptr->txTail >= ethptr->txRingSize) {
		ethptr->txTail = 0;
	}

	/* Length of the packet */
	descptr->buf1size = len;

	/* Copy the packet in the descriptor buffer */
	for(i = 0; i < len; i++) {
		*((char *)descptr->buffer1 + i) = *((char *)buf + i);
	}

	/* If we are at the end of the ring,	*/
	/* indicate end of ring in the desc 	*/
	if(ethptr->txTail == 0) {
		descptr->ctrlstat = ETH_QUARK_TDCS_TER;
	}
	else {
		descptr->ctrlstat = 0;
	}

	/* Initialize the descriptor */
	descptr->ctrlstat |= 
		(ETH_QUARK_TDCS_OWN | /* the desc is owned by DMA	*/
		 ETH_QUARK_TDCS_IC  | /* interrupt after transfer	*/
		 ETH_QUARK_TDCS_LS  | /* last segment of packet		*/
		 ETH_QUARK_TDCS_FS);  /* first segment of packet	*/

	/* Un-suspend the DMA */
	csrptr->tpdr = 1;

	return OK;
}
