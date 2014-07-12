#include <xinu.h>

interrupt quark_eth_intr(struct ether *ethptr) {

	struct	quark_eth_csreg *csrptr = (struct quark_eth_csreg *)ethptr->csr;
						/* Pointer to Ethernet CRSs	*/
	struct	quark_eth_tx_desc *tdescptr;	/* Pointer to tx descriptor	*/
	struct	quark_eth_rx_desc *rdescptr;	/* Pointer to rx descriptor	*/
	uint32	count;				/* Variable used to count packets*/
	
	/* If there is no interrupt pending, return */
	if((csrptr->sr & QUARK_ETH_SR_NIS) == 0) {
		return;
	}

	/* Check the status register to figure out the source of interrupt */

	if(csrptr->sr & QUARK_ETH_SR_TI) { /* Transmit interrupt */

		/* Acknowledge the transmit interrupt */
		csrptr->sr = QUARK_ETH_SR_TI;

		/* Pointer to the head of transmit desc ring */
		tdescptr = (struct quark_eth_tx_desc *)
				((struct quark_eth_tx_desc *)ethptr->txRing + ethptr->txHead);

		/* Initialize the count variable to zero */
		count = 0;

		/* Repeat until we process all the descriptor slots */
		while(ethptr->txHead != ethptr->txTail) {

			/* If the descriptor is owned by DMA, stop here */
			if(tdescptr->ctrlstat & QUARK_ETH_TDCS_OWN) {
				break;
			}

			/* The descriptor is processed by the DMA, increment the count */
			count++;

			/* Go to the next descriptor */
			tdescptr += 1;

			/* Increment the head of the transmit desc ring */
			ethptr->txHead += 1;
			if(ethptr->txHead >= QUARK_ETH_TX_RING_SIZE) {
				ethptr->txHead = 0;
				tdescptr = (struct quark_eth_tx_desc *)ethptr->txRing;
			}
		}

		/* count contains no. of packets processed by DMA, which means
		 * those slots are free for next packets to be transmitted,
		 * signal the semaphore accordingly
		 */
		csrptr->sr = QUARK_ETH_SR_NIS;
		signaln(ethptr->osem, count);
		return;
	}
	if(csrptr->sr & QUARK_ETH_SR_RI) { /* Receive interrupt */

		/* Acknowledge the interrupt */
		csrptr->sr = QUARK_ETH_SR_RI;

		/* Get the pointer to the tail of the receive desc list */
		rdescptr = (struct quark_eth_rx_desc *)
			((struct quark_eth_rx_desc *)ethptr->rxRing + ethptr->rxTail);

		/* Initialize the count variable to zero */
		count = 0;

		/* Repeat until we have received maximum of 32 packets */
		while(count <= QUARK_ETH_RX_RING_SIZE) {

			/* If the descriptor is owned by the DMA, stop here */
			if(rdescptr->status & QUARK_ETH_RDST_OWN) {
				break;
			}

			/* Increment the no. of packets received */
			count++;

			/* Move to the next descriptor */
			rdescptr += 1;

			/* Increment the tail index into the receive desc ring */
			ethptr->rxTail += 1;
			if(ethptr->rxTail >= QUARK_ETH_RX_RING_SIZE) {
				ethptr->rxTail = 0;
				rdescptr = (struct quark_eth_rx_desc *)ethptr->rxRing;
			}
		}

		/* count contains the no. of packets received, signal the input semaphore
		 * with that variable to indicate reception of the packets
		 */
		signaln(ethptr->isem, count);
	}

	/* Acknowledge the normal interrupt summary */
	csrptr->sr = QUARK_ETH_SR_NIS;

	return;
}
