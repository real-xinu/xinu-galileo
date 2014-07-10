/* 3c905C_open.c - _3c905C_open */

#include <xinu.h>

/*------------------------------------------------------------------------
 * _3c905C_open - allocate resources and prepare hardware for transmit 
 * 		   and receive
 *------------------------------------------------------------------------
 */
status	_3c905C_open(
	struct 	ether *ethptr
	)
{
	struct	_3c905C_desc *ringptr;	/* pointer to node on a ring	*/
	uint32	bufptr;			/* pointer to next buffer	*/
	int32	i;
	uint32	config;

	/* Initialize structure pointers */

	ethptr->rxRingSize = _3C905C_RX_RING_SIZE;
	ethptr->txRingSize = _3C905C_TX_RING_SIZE;
	ethptr->isem = semcreate(0);
	ethptr->osem = semcreate(ethptr->txRingSize);

	/* Rings must be aligned to a multiple of 16 bytes */

	ethptr->rxRing = (void *)getmem((ethptr->rxRingSize + 1)
			* _3C905C_PDSIZE);
	ethptr->txRing = (void *)getmem((ethptr->txRingSize + 1)
			* _3C905C_PDSIZE);
	ethptr->rxRing = (void *) (((uint32)ethptr->rxRing + 0xf) & ~0xf);
	ethptr->txRing = (void *) (((uint32)ethptr->txRing + 0xf) & ~0xf);

	/* Buffers are highly recommended to be allocated on cache-line */
	/*  size (32-byte for Celeron 600) 				*/

	ethptr->rxBufs = (void *)getmem((ethptr->rxRingSize + 1) 
			* ETH_BUF_SIZE);
	ethptr->txBufs = (void *)getmem((ethptr->txRingSize + 1)
			* ETH_BUF_SIZE);
	ethptr->rxBufs = (void *)(((uint32)ethptr->rxBufs + 0x1f) 
			& ~0x1f);
	ethptr->txBufs = (void *)(((uint32)ethptr->txBufs + 0x1f)
			& ~0x1f);

	if (  ( (int32)ethptr->rxBufs == SYSERR )
	   || ( (int32)ethptr->txBufs == SYSERR ) ) {
		return SYSERR;
	}

	/* Set buffers and ring entries to zero */

	memset(ethptr->rxBufs, '\0', ethptr->rxRingSize * ETH_BUF_SIZE);
	memset(ethptr->txBufs, '\0', ethptr->txRingSize * ETH_BUF_SIZE);
	memset(ethptr->rxRing, '\0', _3C905C_PDSIZE * ethptr->rxRingSize);
	memset(ethptr->txRing, '\0', _3C905C_PDSIZE * ethptr->txRingSize);

	/* Link the receive rings */

	ringptr = (struct _3c905C_desc *)ethptr->rxRing;
	bufptr = (uint32)ethptr->rxBufs;
	for (i = 0; i < ethptr->rxRingSize; i++) {
		if (i == (ethptr->rxRingSize - 1)) {
			ringptr->next = 
				(struct _3c905C_desc*)ethptr->rxRing;
		} else {
			ringptr->next = ringptr + 1;
		}
		ringptr->buffer_addr = bufptr;
		ringptr->length = _3C905C_FLAG_LASTFRAG | ETH_BUF_SIZE;
		ringptr++;
		bufptr += ETH_BUF_SIZE;
	}

	/* Link the transmit rings*/

	ringptr = (struct _3c905C_desc *)ethptr->txRing;
	bufptr = (uint32)ethptr->txBufs;
	for (i = 0; i < ethptr->txRingSize; i++) {
		ringptr->buffer_addr = bufptr;
		ringptr++;
		bufptr += ETH_BUF_SIZE;
	}
	ethptr->rxHead = ethptr->rxTail = 0;
	ethptr->txHead = ethptr->txTail = 0;

	/* Select the active media port */ 

	config = _3c905C_win_read32(ethptr, 3, _3C905C_WIN3_CONFIG);	

	/* Set duplex as full */

	_3c905C_win_write16(ethptr, 0x20, 3, _3C905C_WIN3_MACCTRL);

	/* Reset the NIC for receive and transmit */

	_3c905C_cmd(ethptr, _3C905C_CMD_TXRESET, 0x03);
	_3c905C_cmd(ethptr, _3C905C_CMD_RXRESET, 0x07);

	/* Clear status report */

	_3c905C_cmd(ethptr, _3C905C_CMD_SETSTATUSENB, 0x00);

	/* Set station mask */

	for (i = 6; i < 12; i += 2)
		_3c905C_win_write16(ethptr, 0, 2, i);

	/* Configure Rx */

	outw(ethptr->iobase + _3C905C_REG_COMMAND,
			(uint32)(_3C905C_CMD_SETRXTHRESHOLD + (1536>>2)));
	outl(ethptr->iobase + _3C905C_MSTCTRL_PKTSTATUS, 0x0020);
	outl(ethptr->iobase + _3C905C_MSTCTRL_UPLISTPTR,
			(uint32)ethptr->rxRing);

	/* Configure TX */

	outl(ethptr->iobase + _3C905C_MSTCTRL_DOWNLISTPTR, 0);

	/* Set reveiver mode: physical and broadcast address only	*/
	_3c905C_cmd(ethptr, _3C905C_CMD_SETRXFILTER, 
			_3C905C_RXFILTER_STATION |
			_3C905C_RXFILTER_BROADCAST);

	/* Enable receiver and transmitter */

	_3c905C_cmd(ethptr, _3C905C_CMD_RXENABLE, 0);
	_3c905C_cmd(ethptr, _3C905C_CMD_TXENABLE, 0);

	/* Set interrupt status register */

	_3c905C_cmd(ethptr, _3C905C_CMD_SETSTATUSENB,
			_3C905C_STATUS_DOWNCOMPLETE |
			_3C905C_STATUS_UPCOMPLETE);
	
	/* Ack all pending events, and set active indicator mask */
	
	_3c905C_cmd(ethptr, _3C905C_CMD_ACKINTR,
			_3C905C_STATUS_INTLATCH);

	/* Enable interrupt */

	set_evec(ethptr->dev->dvirq + IRQBASE, (uint32)ethDispatch);
	_3c905C_cmd(ethptr, _3C905C_CMD_SETINTRENB,
			_3C905C_STATUS_INTLATCH | 
			_3C905C_STATUS_DOWNCOMPLETE |
			_3C905C_STATUS_UPCOMPLETE);
	
	return OK;
}
