/* 3c905C_write.c - _3c905C_write */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethWrite - write a packet to an Ethernet device
 *------------------------------------------------------------------------
 */
devcall _3c905C_write(
	struct 	ether *ethptr, 		/* ptr to entry in ethertab 	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct	_3c905C_desc *currdescptr;/* ptr to curr ring descriptor*/
	struct 	_3c905C_desc *prevdescptr;/* ptr to prev ring descriptor*/
	char 	*pktptr; 		/* ptr used during packet copy 	*/
	uint32	tail;			/* index of ring buffer for pkt	*/
	uint32	prev;			/* index of previous ring buf	*/

	/* Wait for a free ring slot */

	wait(ethptr->osem);

	/* Find the tail of the ring to insert packet */

	prev = (ethptr->txTail - 1) % ethptr->txRingSize;
	prevdescptr = (struct _3c905C_desc *)ethptr->txRing + prev;
	tail = ethptr->txTail;
	currdescptr = (struct _3c905C_desc *)ethptr->txRing + tail;

	/* Copy packet to transmit ring buffer */

	pktptr = (char *)currdescptr->buffer_addr;
	memcpy(pktptr, buf, len);

	/* Insert transmitting command and length */
	
	currdescptr->next = 0;
	currdescptr->status = _3C905C_DESC_TXINTRUPLOADED | len;
	currdescptr->length = _3C905C_FLAG_LASTFRAG | len;
	
	/* Kickout the packet */

	_3c905C_cmd(ethptr, _3C905C_CMD_DOWNSTALL, 0x0000);

	prevdescptr->next = currdescptr;

	if (inl(ethptr->iobase + _3C905C_MSTCTRL_DOWNLISTPTR) == 0)
		outl(ethptr->iobase + _3C905C_MSTCTRL_DOWNLISTPTR,
				(uint32)currdescptr);

	prevdescptr->status &= ~_3C905C_DESC_TXINTRUPLOADED;

	/* Start the transmitter */

	_3c905C_cmd(ethptr, _3C905C_CMD_DOWNUNSTALL, 0x0000);

	/* Advance the ring tail pointing to the next available ring 	*/
	/* 	descriptor 						*/

	ethptr->txTail = (ethptr->txTail + 1) % ethptr->txRingSize;

	return len;
}
