/* e1000e_intr.c - e1000e_intr */

#include <xinu.h>

/*------------------------------------------------------------------------
 * e1000e_rxPackets - handler for receiver interrupts
 *------------------------------------------------------------------------
 */
void 	e1000e_rxPackets(
	struct 	ether 	*ethptr 	/* ptr to control block		*/
	)
{
	struct	e1000e_rx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32	tail;			/* pos to insert next packet	*/
	uint32	status;			/* status of ring descriptor 	*/
	int numdesc; 			/* num. of descriptor reclaimed	*/

	for (numdesc = 0; numdesc < ethptr->rxRingSize; numdesc++) {

		/* Insert new arrived packet to the tail */

		tail = ethptr->rxTail;
		descptr = (struct e1000e_rx_desc *)ethptr->rxRing + tail;
		status = descptr->status;

		if (status == 0) {
			break;
		}

		ethptr->rxTail 
			= (ethptr->rxTail + 1) % ethptr->rxRingSize;
	}

	signaln(ethptr->isem, numdesc);

	return;
}

/*------------------------------------------------------------------------
 * e1000e_txPackets - handler for transmitter interrupts
 *------------------------------------------------------------------------
 */
void 	e1000e_txPackets(
	struct	ether *ethptr		/* ptr to control block		*/
	)
{
	struct	e1000e_tx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32 	head; 			/* pos to reclaim descriptor	*/
	char 	*pktptr; 		/* ptr used during packet copy  */
	int 	numdesc; 		/* num. of descriptor reclaimed	*/

	for (numdesc = 0; numdesc < ethptr->txRingSize; numdesc++) {
		head = ethptr->txHead;
		descptr = (struct e1000e_tx_desc *)ethptr->txRing + head;

		if (!(descptr->upper.data & E1000E_TXD_STAT_DD))
			break;

		/* Clear the write-back descriptor and buffer */

		descptr->lower.data = 0;
		descptr->upper.data = 0;
		pktptr = (char *)((uint32)(descptr->buffer_addr &
					   ADDR_BIT_MASK));
		memset(pktptr, '\0', ETH_BUF_SIZE);

		ethptr->txHead 
			= (ethptr->txHead + 1) % ethptr->txRingSize;
	}

	signaln(ethptr->osem, numdesc);

	return;
}


/*------------------------------------------------------------------------
 * e1000e_intr - decode and handle interrupt from an E1000E device
 *------------------------------------------------------------------------
 */
interrupt e1000e_intr(
	struct 	ether *ethptr
	)
{
	uint32	status;

	/* Disable device interrupt */

	e1000e_irq_disable(ethptr);

	/* Obtain status bits from device */

	status = e1000e_io_readl(ICR);

	/* Not our interrupt */

	if (status == 0) {
		e1000e_irq_enable(ethptr);
		return;
	}

	resched_cntl(DEFER_START);

	if (status & E1000E_ICR_LSC) {
		//kprintf("e1000e_intr: Catch link status change!\n");
	}

	if (status & E1000E_ICR_RXT0) {
		ethptr->rxIrq++;
		e1000e_rxPackets(ethptr);
	}

	if (status & E1000E_ICR_TXDW) {
		ethptr->txIrq++;
		e1000e_txPackets(ethptr);
	}

	/* Enable device interrupt */

	e1000e_irq_enable(ethptr);
	
	resched_cntl(DEFER_STOP);

	return;
}
