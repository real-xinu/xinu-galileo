#include <xinu.h>

devcall quark_eth_write(struct ether *ethptr, void *buf, uint32 len) {

	struct	quark_eth_csreg *csrptr = (struct quark_eth_csreg *)ethptr->csr;
	volatile struct	quark_eth_tx_desc *descptr;
	uint32 i;

	/* Wait for an empty slot in the transmit desc ring */
	wait(ethptr->osem);

	/* Pointer to the tail of the desc ring */
	descptr = (struct quark_eth_tx_desc *)
			((struct quark_eth_tx_desc *)ethptr->txRing + ethptr->txTail);

	/* Increment the tail index */
	ethptr->txTail += 1;
	if(ethptr->txTail >= QUARK_ETH_TX_RING_SIZE) {
		ethptr->txTail = 0;
	}

	/* Length of the packet */
	descptr->buf1size = len;

	/* Copy the packet in the descriptor buffer */
	for(i = 0; i < len; i++) {
		*((char *)descptr->buffer1 + i) = *((char *)buf + i);
	}

	/* If we are at the end of the ring, indicate end of ring in the desc */
	if(ethptr->txTail == 0) {
		descptr->ctrlstat = QUARK_ETH_TDCS_TER;
	}
	else {
		descptr->ctrlstat = 0;
	}

	/* Initialize the descriptor */
	descptr->ctrlstat |= (QUARK_ETH_TDCS_OWN | /* Indicate that the desc is owned by DMA	*/
			      QUARK_ETH_TDCS_IC	 | /* Issue an interrupt after transfer		*/
			      QUARK_ETH_TDCS_LS	 | /* This desc is the last segment of packet	*/
			      QUARK_ETH_TDCS_FS);  /* This desc is the first segment of packet	*/

	/* Un-suspend the DMA */
	csrptr->tpdr = 1;

	return OK;
}
