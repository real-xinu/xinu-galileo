#include <xinu.h>

status quark_eth_open(struct ether *ethptr) {

	void	*temptr;			/* Temporary pointer variable		*/
	struct	quark_eth_csreg *csrptr = (struct quark_eth_csreg *)ethptr->csr;
	struct	quark_eth_tx_desc *tx_descs;	/* Pointer to initialize transmit descs */
	struct	quark_eth_rx_desc *rx_descs;	/* Pointer to initialize receive descs  */
	struct	netpacket *pktptr;		/* Pointer to packet buffers		*/
	int32	i;				/* Variable to index through descriptors*/

	/* Allocate memory for the transmit ring */
	temptr = (void *)getmem(sizeof(struct quark_eth_tx_desc) *
				(QUARK_ETH_TX_RING_SIZE+1));
	if((int)temptr == SYSERR) {
		return SYSERR;
	}
	memset(temptr, 0, sizeof(struct quark_eth_tx_desc) *
				(QUARK_ETH_TX_RING_SIZE+1));

	/* The transmit descriptors need to be 4-byte aligned */
	ethptr->txRing = (void *)(((uint32)temptr + 3) & (~3));

	/* Allocate memory for transmit buffers */
	ethptr->txBufs = (void *)getmem(sizeof(struct netpacket) *
				(QUARK_ETH_TX_RING_SIZE+1));
	if((int)ethptr->txBufs == SYSERR) {
		return SYSERR;
	}
	ethptr->txBufs = (void *)(((uint32)ethptr->txBufs + 3) & (~3));

	/* Pointers to initialize transmit descriptors */
	tx_descs = (struct quark_eth_tx_desc *)ethptr->txRing;
	pktptr = (struct netpacket *)ethptr->txBufs;

	/* Initialize the transmit descriptors */
	for(i = 0; i < QUARK_ETH_TX_RING_SIZE; i++) {
		tx_descs[i].buffer1 = (uint32)(pktptr + i);
	}

	/* Create the output synchronization semaphore */
	ethptr->osem = semcreate(QUARK_ETH_TX_RING_SIZE);
	if((int)ethptr->osem == SYSERR) {
		return SYSERR;
	}

	/* Allocate memory for the receive descriptors */
	temptr = (void *)getmem(sizeof(struct quark_eth_rx_desc) *
				(QUARK_ETH_RX_RING_SIZE+1));
	if((int)temptr == SYSERR) {
		return SYSERR;
	}
	memset(temptr, 0, sizeof(struct quark_eth_rx_desc) *
				(QUARK_ETH_RX_RING_SIZE+1));

	/* Receive descriptors must be 4-byte aligned */
	ethptr->rxRing = (struct quark_eth_rx_desc *)(((uint32)temptr + 3) & (~3));

	/* Allocate memory for the receive buffers */
	ethptr->rxBufs = (void *)getmem(sizeof(struct netpacket) *
					(QUARK_ETH_RX_RING_SIZE+1));
	if((int)ethptr->rxBufs == SYSERR) {
		return SYSERR;
	}

	/* Receive buffers must be 4-byte aligned */
	ethptr->rxBufs = (void *)(((uint32)ethptr->rxBufs + 3) & (~3));

	/* Pointer to initialize receive descriptors */
	rx_descs = (struct quark_eth_rx_desc *)ethptr->rxRing;

	/* Pointer to data buffers */
	pktptr = (struct netpacket *)ethptr->rxBufs;

	/* Initialize the receive descriptors */
	for(i = 0; i < QUARK_ETH_RX_RING_SIZE; i++) {

		rx_descs[i].status   = QUARK_ETH_RDST_OWN;
		rx_descs[i].buf1size = (uint32)sizeof(struct netpacket);
		rx_descs[i].buffer1  = (uint32)(pktptr + i);
	}

	/* Indicate end of ring on last descriptor */
	rx_descs[QUARK_ETH_RX_RING_SIZE-1].buf1size |= (QUARK_ETH_RDCTL1_RER);

	/* Create the input synchronization semaphore */
	ethptr->isem = semcreate(0);
	if((int)ethptr->isem == SYSERR) {
		return SYSERR;
	}

	/* Enable the Transmit and Receive Interrupts */
	csrptr->ier = (QUARK_ETH_IER_NIE | QUARK_ETH_IER_TIE | QUARK_ETH_IER_RIE);

	/* Initialize the transmit descriptor base address */
	csrptr->tdla = (uint32)ethptr->txRing;

	/* Initialize the receive descriptor base address */
	csrptr->rdla = (uint32)ethptr->rxRing;

	/* Enable the MAC Receiver and Transmitter */
	csrptr->maccr |= (QUARK_ETH_MACCR_TE | QUARK_ETH_MACCR_RE);

	/* Start the Transmit and Receive Processes in the DMA */
	csrptr->omr |= (QUARK_ETH_OMR_ST | QUARK_ETH_OMR_SR);

	return OK;
}
