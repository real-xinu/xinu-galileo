/* ethhandler.c - ethhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethhandler  -  Interrupt handler for Intel Quark Ethernet
 *------------------------------------------------------------------------
 */
interrupt	ethhandler(void)
{
	struct	ethcblk	*ethptr;	/* Ethertab entry pointer	*/
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

	if (csrptr->sr & ETH_QUARK_SR_TI) { /* Transmit interrupt */

		/* Acknowledge the transmit interrupt */

		csrptr->sr = ETH_QUARK_SR_TI;

		/* Pointer to the head of transmit desc ring */

		tdescptr = (struct eth_q_tx_desc *)ethptr->txRing +
							ethptr->txHead;

		count = 0;	/* Start packet count at zero */

		/* Repeat until we process all the descriptor slots */

		while(ethptr->txHead != ethptr->txTail) {

			/* If the descriptor is owned by DMA, stop here */

			if(tdescptr->ctrlstat & ETH_QUARK_TDCS_OWN) {
				break;
			}

			/* Descriptor was processed; increment count	*/
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

		/* 'count' packets were processed by DMA, and slots are	*/
		/* now free; signal the semaphore accordingly		*/

		csrptr->sr = ETH_QUARK_SR_NIS;
		signaln(ethptr->osem, count);
		return;
	} else if(csrptr->sr & ETH_QUARK_SR_RI) { /* Receive interrupt	*/

		/* Acknowledge the interrupt */

		csrptr->sr = ETH_QUARK_SR_RI;

		/* Get the pointer to the tail of the receive desc list */

		rdescptr = (struct eth_q_rx_desc *)ethptr->rxRing +
							ethptr->rxTail;

		count = 0;	/* Start packet count at zero */

		/* Repeat until we have received		*/
		/* maximum no. packets that can fit in queue 	*/

		while(count <= ethptr->rxRingSize) {

			/* If the descriptor is owned by the DMA, stop */

			if(rdescptr->status & ETH_QUARK_RDST_OWN) {
				break;
			}

			/* Descriptor was processed; increment count	*/
			count++;

			/* Go to the next descriptor */

			rdescptr += 1;

			/* Increment the tail index of the rx desc ring */

			ethptr->rxTail += 1;
			if(ethptr->rxTail >= ethptr->rxRingSize) {
				ethptr->rxTail = 0;
				rdescptr = (struct eth_q_rx_desc *)
							ethptr->rxRing;
			}
		}

		/* 'count' packets were received and are available,	*/
		/*   so signal the semaphore accordingly		*/

		signaln(ethptr->isem, count);
	}

	/* Acknowledge the normal interrupt summary */

	csrptr->sr = ETH_QUARK_SR_NIS;

	return;
}
