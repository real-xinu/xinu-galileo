/* 3c905C_read.c  - _3c905C_read */

#include <xinu.h>

/*------------------------------------------------------------------------
 * _3c905C_read - read a packet from an 3C905C device
 *------------------------------------------------------------------------
 */
devcall	_3c905C_read(
	struct 	ether *ethptr, 		/* ptr to entry in ethertab 	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct	_3c905C_desc *descptr;	/* ptr to ring buffer 		*/
	char	*pktptr;		/* ptr used during packet copy	*/
	uint32	head;			/* head of ring buffer 		*/
	uint32	status;			/* status of entry		*/
	uint32	length;			/* packet length		*/
	int32 	retval;

	/* Wait for a packet to arrive */

	wait(ethptr->isem);
	
	/* Find out where to pick up the packet */

	head = ethptr->rxHead;
	descptr = (struct _3c905C_desc *)ethptr->rxRing + head;
	status = descptr->status;

	if (status & _3C905C_DESC_RXDERROR) { 	/* check for error */
		kprintf("_3c905C_read: packet error!\n");
		retval = SYSERR;
	} else { 	/* pick up the packet */
		pktptr = (char *)descptr->buffer_addr;
		length = descptr->status & 0x1fff;
		memcpy(buf, pktptr, length);
		retval = length;
	}

	/* Clear up the descriptor and the buffer */

	descptr->status = 0;
	descptr->length = _3C905C_FLAG_LASTFRAG | ETH_BUF_SIZE;	
	memset((char *)descptr->buffer_addr, '\0', ETH_BUF_SIZE);

	ethptr->rxHead = (ethptr->rxHead + 1) % ethptr->rxRingSize;

	return retval;
}
