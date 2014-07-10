/* e1000e_read.c - e1000e_read */

#include <xinu.h>

/*------------------------------------------------------------------------
 * e1000e_read - read a packet from an E1000E device
 *------------------------------------------------------------------------
 */
devcall	e1000e_read(
	struct	ether *ethptr,		/* ptr to entry in ethertab	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct	e1000e_rx_desc *descptr;/* ptr to ring descriptor	*/
	char	*pktptr;		/* ptr used during packet copy	*/
	uint32	head;			/* head of ring buffer 		*/
	uint32	status;			/* status of entry		*/
	uint32	length;			/* packet length		*/
	int32 	retval;
	uint32 	rdt;

	/* Wait for a packet to arrive */

	wait(ethptr->isem);

	/* Find out where to pick up the packet */

	head = ethptr->rxHead;
	descptr = (struct e1000e_rx_desc *)ethptr->rxRing + head;
	status = descptr->status;

	if (!(status & E1000E_RXD_STAT_DD)) { 	/* check for error */
		kprintf("e1000e_read: packet error!\n");
		retval = SYSERR;
	} else { 	/* pick up the packet */			
		pktptr = (char *)((uint32)(descptr->buffer_addr &
					   ADDR_BIT_MASK));
		length = descptr->length;
		memcpy(buf, pktptr, length);
		retval = length;
	}
	/* Clear up the descriptor and the buffer */

	descptr->length = 0;
	descptr->csum = 0;
	descptr->status = 0;
	descptr->errors = 0;
	descptr->special = 0;
	memset((char *)((uint32)(descptr->buffer_addr & ADDR_BIT_MASK)), 
			'\0', ETH_BUF_SIZE); 

	/* Add newly reclaimed descriptor to the ring */

	if (ethptr->rxHead % E1000E_RING_BOUNDARY == 0) {
		rdt = e1000e_io_readl(RDT(0));
		rdt = (rdt + E1000E_RING_BOUNDARY) % ethptr->rxRingSize;
		e1000e_io_writel(RDT(0), rdt);
	}

	/* Advance the head pointing to the next ring descriptor which 	*/
	/*  	will be ready to be picked up 				*/
	ethptr->rxHead = (ethptr->rxHead + 1) % ethptr->rxRingSize;

	return retval;
}
