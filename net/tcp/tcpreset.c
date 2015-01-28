/* tcpreset.c  -  tcpreset */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpreset  -  Send a TCP RESET segment
 *------------------------------------------------------------------------
 */
int32	tcpreset(
	  struct netpacket *oldpkt	/* Ptr to old packet		*/
	)
{
	struct netpacket *pkt;		/* Pointer to new packet	*/
	uint32		lip;		/* Local IP address to use	*/
	uint32		rip;		/* Remote IP address (reply)	*/
	int32		len;		/* Length of the TCP data	*/

	/* Did we already send a RESET? */

	if (oldpkt->net_tcpcode & TCPF_RST)
		return SYSERR;

	/* Allocate a buffer */

	pkt = (struct netpacket *)getbuf(netbufpool);

	if ((int32)pkt == SYSERR) {
		return SYSERR;
	}

	/* Compute length of TCP data (needed for ACK) */

	len = oldpkt->net_iplen - IP_HDR_LEN - TCP_HLEN(pkt);

	/* Obtain remote IP address */

	rip = oldpkt->net_ipsrc;
	lip = oldpkt->net_ipdst;

	/* Create TCP packet in pkt */

	memcpy((char *)pkt->net_ethsrc, NetData.ethucast, ETH_ADDR_LEN);
	pkt->net_ethtype = 0x0800;	/* Type is IP */

	/* Fill in IP header */

	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= IP_HDR_LEN + TCP_HLEN(pkt);
					/* total datagram length	*/
	pkt->net_ipid = 0;		/* datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_TCP;	/* datagram carries TCP		*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = lip;		/* IP source address		*/
	pkt->net_ipdst = rip;		/* IP destination address	*/

	/* Fill in TCP header */

	pkt->net_tcpsport = oldpkt->net_tcpdport;
	pkt->net_tcpdport = oldpkt->net_tcpsport;
	if (oldpkt->net_tcpcode & TCPF_ACK) {
		pkt->net_tcpseq = oldpkt->net_tcpack;
	} else {
		pkt->net_tcpseq = 0;
	}
	pkt->net_tcpack = oldpkt->net_tcpseq + len;
	if (oldpkt->net_tcpcode & (TCPF_SYN|TCPF_FIN)) {
		pkt->net_tcpack++;
	}
	pkt->net_tcpcode = TCP_HDR_LEN << 10;
	pkt->net_tcpcode |= TCPF_ACK | TCPF_RST;
	pkt->net_tcpwindow = 0;
	pkt->net_tcpcksum = 0;
	pkt->net_tcpurgptr = 0;

	/* Call ip_send to send the datagram */

	ip_enqueue(pkt);
	return OK;
}
