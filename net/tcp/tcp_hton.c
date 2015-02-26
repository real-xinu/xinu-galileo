/* tcp_hton.c  -  tcp_hton, tcp_ntoh */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcp_hton  -  convert TCP fields from host byte order to network order
 *------------------------------------------------------------------------
 */
void	tcp_hton(
	  struct netpacket *pkt		/* Pointer to a TCP segment	*/
	)
{
	pkt->net_tcpsport = htons (pkt->net_tcpsport);
	pkt->net_tcpdport = htons (pkt->net_tcpdport);
	pkt->net_tcpseq = htonl (pkt->net_tcpseq);
	pkt->net_tcpack = htonl (pkt->net_tcpack);
	pkt->net_tcpcode = htons (pkt->net_tcpcode);
	pkt->net_tcpwindow = htons (pkt->net_tcpwindow);
	pkt->net_tcpurgptr = htons (pkt->net_tcpurgptr);
	return;
}


/*------------------------------------------------------------------------
 *  tcp_ntoh  -  convert TCP fields from network byte order to host order
 *------------------------------------------------------------------------
 */
void	tcp_ntoh(
	  struct netpacket *pkt		/* Pointer to a TCP segment	*/
	)
{
	pkt->net_tcpsport = ntohs (pkt->net_tcpsport);
	pkt->net_tcpdport = ntohs (pkt->net_tcpdport);
	pkt->net_tcpseq = ntohl (pkt->net_tcpseq);
	pkt->net_tcpack = ntohl (pkt->net_tcpack);
	pkt->net_tcpcode = ntohs (pkt->net_tcpcode);
	pkt->net_tcpwindow = ntohs (pkt->net_tcpwindow);
	pkt->net_tcpurgptr = ntohs (pkt->net_tcpurgptr);
	return;
}
