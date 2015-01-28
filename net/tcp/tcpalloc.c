/* tcpalloc.c  -  tcpalloc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpalloc  -  allocate a buffer for an IP datagram carrying TCP
 *------------------------------------------------------------------------
 */
struct netpacket *tcpalloc(
	  struct tcb	*tcbptr,	/* Ptr to a TCB			*/
	  int32		len		/* Length of the TCP segment	*/
	)
{
	struct netpacket *pkt;		/* Ptr to packet for new struct	*/

	/* Allocate a buffer for a TCB */

	pkt = (struct netpacket *) getbuf(netbufpool);

	/* Set Ethernet type */

	/* Fill in Ethernet header */

	memcpy((char *)pkt->net_ethsrc, NetData.ethucast, ETH_ADDR_LEN);
	pkt->net_ethtype = 0x0800;	/* Type is IP */

	/* Fill in IP header */

	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= IP_HDR_LEN + TCP_HDR_LEN + len;
					/* Total datagram length	*/
	pkt->net_ipid = 0;		/* datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_TCP;	/* datagram carries TCP		*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = tcbptr->tcb_lip;/* IP source address		*/
	pkt->net_ipdst = tcbptr->tcb_rip;/* IP destination address	*/

	/* Set the TCP port fields in the segment */

	pkt->net_tcpsport = tcbptr->tcb_lport;
	pkt->net_tcpdport = tcbptr->tcb_rport;

	/* Set the code field (includes Hdr length */

	pkt->net_tcpcode = (TCP_HDR_LEN << 10);

	if (tcbptr->tcb_state > TCB_SYNSENT) {
		pkt->net_tcpcode |= TCPF_ACK;
		pkt->net_tcpack = tcbptr->tcb_rnext;
	}

	/* Set the window advertisement */

	pkt->net_tcpwindow = tcbptr->tcb_rbsize - tcbptr->tcb_rblen;

	/* Silly Window Avoidance belongs here... */

	/* Initialize the checksum and urgent pointer */
	pkt->net_tcpcksum = 0;
	pkt->net_tcpurgptr = 0;

	/* Return entire packet to caller */

	return pkt;
}
