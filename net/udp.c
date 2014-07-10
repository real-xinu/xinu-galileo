/* udp.c - udp_init, udp_in, udp_register, udp_send, udp_sendto,	*/
/*	        udp_recv, udp_recvaddr, udp_release, udp_ntoh, udp_hton	*/

#include <xinu.h>

struct	udpentry udptab[UDP_SLOTS];		/* table of UDP endpts	*/

/*------------------------------------------------------------------------
 * udp_init - initialize UDP endpoint table
 *------------------------------------------------------------------------
 */
void	udp_init(void)
{

	int32	i;			/* table index */

	for(i=0; i<UDP_SLOTS; i++) {
		udptab[i].udstate = UDP_FREE;
	}

	return;
}


/*------------------------------------------------------------------------
 * udp_in - handle an incoming UDP packet
 *------------------------------------------------------------------------
 */
void	udp_in(
	  struct netpacket *pktptr	/* ptr to  current packet	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* ptr to udptab entry	*/


	/* Insure only one process can access the UDP table at a time	*/

	mask = disable();

	for (i=0; i<UDP_SLOTS; i++) {
	    udptr = &udptab[i];
	    if (udptr->udstate == UDP_FREE) {
			continue;
	    }

	    if ((pktptr->net_udpdport == udptr->udlocport)  &&
                    ((udptr->udremport == 0) ||
                        (pktptr->net_udpsport == udptr->udremport)) &&
                 (  ((udptr->udremip==0)     ||
                        (pktptr->net_ipsrc == udptr->udremip)))    ) {

		/* Entry matches incoming packet */

		if (udptr->udcount < UDP_QSIZ) {
			udptr->udcount++;
			udptr->udqueue[udptr->udtail++] = pktptr;
			if (udptr->udtail >= UDP_QSIZ) {
				udptr->udtail = 0;
			}
			if (udptr->udstate == UDP_RECV) {
				udptr->udstate = UDP_USED;
				send (udptr->udpid, OK);
			}
			restore(mask);
			return;
		}
	    }
	}

	/* No match - simply discard packet */

	freebuf((char *) pktptr);
	restore(mask);
	return;
}

/*------------------------------------------------------------------------
 * udp_register - register a remote (IP,port) and local (interface, port)
 *		  to receive incoming UDP messages from the specified
 *		  remote site sent to the specified local port
 *------------------------------------------------------------------------
 */
uid32	udp_register (
	 uint32	remip,			/* remote IP address or zero	*/
	 uint16	remport,		/* remote UDP protocol port	*/
	 uint16	locport			/* local UDP protocol port	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	int32	slot;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/

	/* Insure only one process can access the UDP table at a time	*/

	mask = disable();

	/* See if request already registered */

	for (slot=0; slot<UDP_SLOTS; slot++) {
		udptr = &udptab[slot];
		if (udptr->udstate == UDP_FREE) {
			continue;
		}

		/* Look at this entry in table */

		if ( (remport == udptr->udremport) &&
		     (locport == udptr->udlocport) &&
		     (remip   == udptr->udremip  ) ) {

			/* Request already in the table */

			restore(mask);
			return SYSERR;
		}
	}

	/* Find a free slot and allocate it */

	for (slot=0; slot<UDP_SLOTS; slot++) {
		udptr = &udptab[slot];
		if (udptr->udstate != UDP_FREE) {
			continue;
		}
		udptr->udlocport = locport;
		udptr->udremport = remport;
		udptr->udremip = remip;
		udptr->udcount = 0;
		udptr->udhead = udptr->udtail = 0;
		udptr->udpid = -1;
		udptr->udstate = UDP_USED;
		restore(mask);
		return slot;
	}

	restore(mask);
	return SYSERR;
}

/*------------------------------------------------------------------------
 * udp_recv - receive a UDP packet
 *------------------------------------------------------------------------
 */
int32	udp_recv (
	 uid32	slot,			/* slot in table to use		*/
	 char   *buff,			/* buffer to hold UDP data	*/
	 int32	len,			/* length of buffer		*/
	 uint32	timeout			/* read timeout in msec		*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	umsg32	msg;			/* message from recvtime()	*/
	struct	netpacket *pkt;		/* ptr to packet being read	*/
	int32	i;			/* counts bytes			*/
	int32	msglen;			/* length of UDP data in packet	*/
	char	*udataptr;		/* pointer to UDP data		*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* Verify that the slot is valid */

	if ((slot < 0) || (slot >= UDP_SLOTS)) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate != UDP_USED) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	if (udptr->udcount == 0) {		/* No packet is waiting	*/
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a packet	*/
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
	if (udptr->udhead >= UDP_QSIZ) {
		udptr->udhead = 0;
	}
	udptr->udcount--;

	/* Copy UDP data from packet into caller's buffer */

	msglen = pkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)pkt->net_udpdata;
	if (len < msglen) {
		msglen = len;
	}
	for (i=0; i<msglen; i++) {
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return msglen;
}

/*------------------------------------------------------------------------
 * udp_recvaddr - receive a UDP packet and record the sender's address
 *------------------------------------------------------------------------
 */
int32	udp_recvaddr (
	 uid32	slot,			/* slot in table to use		*/
	 uint32	*remip,			/* loc for remote IP address	*/
	 uint16	*remport,		/* loc for remote protocol port	*/
	 char   *buff,			/* buffer to hold UDP data	*/
	 int32	len,			/* length of buffer		*/
	 uint32	timeout			/* read timeout in msec		*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	umsg32	msg;			/* message from recvtime()	*/
	struct	netpacket *pkt;		/* ptr to packet being read	*/
	int32	msglen;			/* length of UDP data in packet	*/
	int32	i;			/* counts bytes			*/
	char	*udataptr;		/* pointer to UDP data		*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* Verify that the slot is valid */

	if ((slot < 0) || (slot >= UDP_SLOTS)) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate != UDP_USED) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	if (udptr->udcount == 0) {		/* No packet is waiting */
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a packet	*/
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
	if (udptr->udhead >= UDP_QSIZ) {
		udptr->udhead = 0;
	}

	/* Record sender's IP address and UDP port number */

	*remip = pkt->net_ipsrc;
	*remport = pkt->net_udpsport;

	udptr->udcount--;

	/* Copy UDP data from packet into caller's buffer */

	msglen = pkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)pkt->net_udpdata;
	if (len < msglen) {
		msglen = len;
	}
	for (i=0; i<msglen; i++) {
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return msglen;
}

/*------------------------------------------------------------------------
 * udp_send - send a UDP packet using info in a UDP table entry
 *------------------------------------------------------------------------
 */
status	udp_send (
	 uid32	slot,			/* table slot to use		*/
	 char   *buff,			/* buffer of UDP data		*/
	 int32	len			/* length of data in buffer	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	netpacket *pkt;		/* ptr to packet buffer		*/
	int32	pktlen;			/* total packet length		*/
	static	uint16 ident = 1;	/* datagram IDENT field		*/
	char	*udataptr;		/* pointer to UDP data		*/
	uint32	remip;			/* remote IP address to use	*/
	uint16	remport;		/* remote protocol port to use	*/
	uint16	locport;		/* local protocol port to use	*/
	uint32	locip;			/* local IP address taken from	*/
					/*   the interface		*/
	struct	udpentry *udptr;	/* ptr to table entry		*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that the slot has a specified remote address */

	remip = udptr->udremip;
	if (remip == 0) {
		restore(mask);
		return SYSERR;
	}

	locip = NetData.ipucast;
	remport = udptr->udremport;
	locport = udptr->udlocport;

	/* Allocate a network buffer to hold the packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	/* Compute packet length as UDP data size + fixed header size	*/

	pktlen = ((char *)&pkt->net_udpdata - (char *)pkt) + len;

	/* Create UDP packet in pkt */

	memcpy((char *)pkt->net_ethsrc,NetData.ethucast,ETH_ADDR_LEN);
	pkt->net_ethtype = 0x0800;	/* Type is IP */
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= pktlen - ETH_HDR_LEN;/* total IP datagram length*/
	pkt->net_ipid = ident++;	/* datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_UDP;	/* datagram carries UDP		*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = locip;		/* IP source address		*/
	pkt->net_ipdst = remip;		/* IP destination address	*/

	pkt->net_udpsport = locport;	/* local UDP protocol port	*/
	pkt->net_udpdport = remport;	/* remote UDP protocol port	*/
	pkt->net_udplen = (uint16)(UDP_HDR_LEN+len); /* UDP length	*/
	pkt->net_udpcksum = 0x0000;	/* ignore UDP checksum		*/
	udataptr = (char *) pkt->net_udpdata;
	for (; len>0; len--) {
		*udataptr++ = *buff++;
	}

	/* Call ipsend to send the datagram */

	ip_send(pkt);
	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 * udp_sendto - send a UDP packet to a specified destination
 *------------------------------------------------------------------------
 */
status	udp_sendto (
	 uid32	slot,			/* UDP table slot to use	*/
	 uint32	remip,			/* remote IP address to use	*/
	 uint16	remport,		/* remote protocol port to use	*/
	 char   *buff,			/* buffer of UDP data		*/
	 int32	len			/* length of data in buffer	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	netpacket *pkt;		/* ptr to packet buffer		*/
	int32	pktlen;			/* total packet length		*/
	static	uint16 ident = 1;	/* datagram IDENT field		*/
	struct	udpentry *udptr;	/* prt to UDP table entry	*/
	char	*udataptr;		/* pointer to UDP data		*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Allocate a network buffer to hold the packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	/* Compute packet length as UDP data size + fixed header size	*/

	pktlen = ((char *)&pkt->net_udpdata - (char *)pkt) + len;

	/* Create UDP packet in pkt */

	memcpy((char *)pkt->net_ethsrc,NetData.ethucast,ETH_ADDR_LEN);
        pkt->net_ethtype = 0x0800;	/* Type is IP */
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= pktlen - ETH_HDR_LEN;/* total IP datagram length*/
	pkt->net_ipid = ident++;	/* datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_UDP;	/* datagram carries UDP		*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = NetData.ipucast;/* IP source address		*/
	pkt->net_ipdst = remip;		/* IP destination address	*/
	pkt->net_udpsport = udptr->udlocport;/* local UDP protocol port	*/
	pkt->net_udpdport = remport;	/* remote UDP protocol port	*/
	pkt->net_udplen = (uint16)(UDP_HDR_LEN+len); /* UDP length	*/
	pkt->net_udpcksum = 0x0000;	/* ignore UDP checksum		*/
	udataptr = (char *) pkt->net_udpdata;
	for (; len>0; len--) {
		*udataptr++ = *buff++;
	}

	/* Call ipsend to send the datagram */

	ip_send(pkt);
	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 * udp_release - release a previously-registered UDP slot
 *------------------------------------------------------------------------
 */
status	udp_release (
	 uid32	slot			/* table slot to release	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	struct	netpacket *pkt;		/* ptr to packet being read	*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* Verify that the slot is valid */

	if ( (slot < 0) || (slot >= UDP_SLOTS) ) {
		restore(mask);
		return SYSERR;
	}

	/* Get pointer to table entry */

	udptr = &udptab[slot];

	/* Verify that the slot has been registered and is valid */

	if (udptr->udstate == UDP_FREE) {
		restore(mask);
		return SYSERR;
	}

	resched_cntl(DEFER_START);
	while (udptr->udcount > 0) {
		pkt = udptr->udqueue[udptr->udhead++];
		if (udptr->udhead >= UDP_QSIZ) {
			udptr->udhead = 0;
		}
		freebuf((char *)pkt);
		udptr->udcount--;
	}
	udptr->udstate = UDP_FREE;
	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * udp_ntoh - convert UDP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
void 	udp_ntoh(
	  struct netpacket *pktptr
	)
{
	pktptr->net_udpsport = ntohs(pktptr->net_udpsport);
	pktptr->net_udpdport = ntohs(pktptr->net_udpdport);
	pktptr->net_udplen = ntohs(pktptr->net_udplen);
	return;
}

/*------------------------------------------------------------------------
 * udp_hton - convert packet header fields from host to net byte order
 *------------------------------------------------------------------------
 */
void 	udp_hton(
	  struct netpacket *pktptr
	)
{
	pktptr->net_udpsport = htons(pktptr->net_udpsport);
	pktptr->net_udpdport = htons(pktptr->net_udpdport);
	pktptr->net_udplen = htons(pktptr->net_udplen);
	return;
}
