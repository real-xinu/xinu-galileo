/* eth_q_intr.c - eth_q_intr */

#include <xinu.h>

/*------------------------------------------------------------------------
 * eth_q_intr - interrupt handler for Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
interrupt	eth_q_intr()
{
	struct	ether *ethptr;		/* Ethertab entry pointer	*/
	struct	eth_q_csreg *csrptr;	/* Pointer to Ethernet CRSs	*/
	struct	eth_q_tx_desc *tdescptr;/* Pointer to tx descriptor	*/
	struct	eth_q_rx_desc *rdescptr;/* Pointer to rx descriptor	*/
	uint32	count;			/* Variable used to count pkts	*/

	ethptr = &ethertab[devtab[ETHER0].dvminor];

	csrptr = (struct eth_q_csreg *)ethptr->csr;

	/* If there is no interrupt pending, return */
	if((csrptr->sr & ETH_QUARK_SR_NIS) == 0) {
		return;
	}

	/* Check status register to figure out the source of interrupt */

	if(csrptr->sr & ETH_QUARK_SR_TI) { /* Transmit interrupt */

		/* Acknowledge the transmit interrupt */
		csrptr->sr = ETH_QUARK_SR_TI;

		/* Pointer to the head of transmit desc ring */
		tdescptr = (struct eth_q_tx_desc *)ethptr->txRing +
							ethptr->txHead;

		/* Initialize the count variable to zero */
		count = 0;

		/* Repeat until we process all the descriptor slots */
		while(ethptr->txHead != ethptr->txTail) {

			/* If the descriptor is owned by DMA, stop here */
			if(tdescptr->ctrlstat & ETH_QUARK_TDCS_OWN) {
				break;
			}

			/* The descriptor is processed by	*/
			/* the DMA, increment the count 	*/
			count++;

			/* Go to the next descriptor */
			tdescptr += 1;

			/* Increment the head of the transmit desc ring */
			ethptr->txHead += 1;
			if(ethptr->txHead >= ethptr->txRingSize) {
				ethptr->txHead = 0;
				tdescptr = (struct eth_q_tx_desc *)
							ethptr->txRing;
			}
		}

		/* count contains no. of packets processed by DMA,	*/
		/* which means those slots are free for next packets 	*/
		/* to be transmitted, signal the semaphore accordingly	*/
		csrptr->sr = ETH_QUARK_SR_NIS;
		signaln(ethptr->osem, count);
		return;
	}
	if(csrptr->sr & ETH_QUARK_SR_RI) { /* Receive interrupt */

		/* Acknowledge the interrupt */
		csrptr->sr = ETH_QUARK_SR_RI;

		/* Get the pointer to the tail of the receive desc list */
		rdescptr = (struct eth_q_rx_desc *)ethptr->rxRing +
							ethptr->rxTail;

		/* Initialize the count variable to zero */
		count = 0;

		/* Repeat until we have received maximum of 32 packets */
		while(count <= ethptr->rxRingSize) {

			/* If the descriptor is owned by the DMA, stop */
			if(rdescptr->status & ETH_QUARK_RDST_OWN) {
				break;
			}

			/* Increment the no. of packets received */
			count++;

			/* Move to the next descriptor */
			rdescptr += 1;

			/* Increment the tail index of the rx desc ring */
			ethptr->rxTail += 1;
			if(ethptr->rxTail >= ethptr->rxRingSize) {
				ethptr->rxTail = 0;
				rdescptr = (struct eth_q_rx_desc *)
							ethptr->rxRing;
			}
		}

		/* count contains the no. of packets received,	*/
		/* signal the input semaphore with that variable*/
		/* to indicate reception of the packets		*/
		signaln(ethptr->isem, count);
	}

	/* Acknowledge the normal interrupt summary */
	csrptr->sr = ETH_QUARK_SR_NIS;

	return;
}
