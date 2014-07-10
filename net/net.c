/* net.c - net_init, netin, eth_hton */

#include <xinu.h>
#include <stdio.h>

struct	network	NetData;
bpid32	netbufpool;
/*------------------------------------------------------------------------
 * net_init - initialize network data structures and processes
 *------------------------------------------------------------------------
 */

void	net_init (void)
{

	/* Initialize interface data structures */

	memset((char *)&NetData, NULLCH, sizeof(struct network));

	control(ETHER0, ETH_CTRL_GET_MAC, (int32)NetData.ethucast, 0);

	memset((char *)NetData.ethbcast, 0xFF, ETH_ADDR_LEN);
	
	/* Initialize ARP cache for interface */

	arp_init();

	/* Initialize UDP */

	udp_init();

	/* Initialize ICMP */

	icmp_init();

	/* Initialize the IP output queue */

	ipoqueue.iqhead = 0;
	ipoqueue.iqtail = 0;
	ipoqueue.iqsem = semcreate(0);
	if((int32)ipoqueue.iqsem == SYSERR) {
		panic("Cannot create ip output queue semaphore");
		return;
	}

	/* Create the IP output process */

	resume(create(ipout, NETSTK, NETPRIO, "ipout", 0, NULL));

	/* Create a network input process for each interface */

	resume(create(netin, NETSTK, NETPRIO, "netin", 0, NULL));
}


/*------------------------------------------------------------------------
 * netin - repeatedly read and process the next incoming packet
 *------------------------------------------------------------------------
 */

process	netin ()
{
	struct	netpacket *pkt;	/* ptr to current packet	*/
	int32	nbufs;		/* total no of buffers		*/
	int32	retval;		/* return value from read	*/

	nbufs = UDP_SLOTS * UDP_QSIZ + ICMP_SLOTS * ICMP_QSIZ + 1;

	netbufpool = mkbufpool(PACKLEN, nbufs);

	/* Do forever: read packets from the network and process */

	pkt = (struct netpacket *)getbuf(netbufpool);
	while(1) {

		/* Obtain next packet arriving on an interface */

		retval = read(ETHER0, (char *)pkt, PACKLEN);
		if(retval == SYSERR) {
			panic("Cannot read from Ethernet\n");
		}

		/* Convert Ethernet Type to host order */

		eth_ntoh(pkt);

		/* Demultiplex on Ethernet type */

		switch (pkt->net_ethtype) {

		    case ETH_ARP:			/* Handle ARP	*/
			arp_in((struct arppacket *)pkt);
			continue;

		    case ETH_IP:			/* Handle IP	*/
			ip_in(pkt);
			continue;
	
		    case ETH_IPv6:			/* Handle IPv6	*/
			freebuf((char *)pkt);
			continue;

		    default:	/* Ignore all other incoming packets	*/
			freebuf((char *)pkt);
			continue;
		}

		pkt = (struct netpacket *)getbuf(netbufpool);
	}
}
#if 0
/*------------------------------------------------------------------------
 * rawin - continuously read the next incoming frame, examine the MAC
 *		address, and enqueue on the appropriate interface queue
 *------------------------------------------------------------------------
 */

process	rawin (void) {

	status	retval;			/* return value from function	*/
	struct	netpacket *pkt;		/* packet buffer being used now	*/
	int32	nbufs;			/* total number of buffers	*/
	struct	ifentry	*ifptr;		/* ptr to interface table entry	*/
	int32	iface;			/* index into interface table	*/

	/* Global buffer pool shared by all interfaces */

	nbufs = UDP_SLOTS * UDP_QSIZ + ICMP_SLOTS * ICMP_QSIZ
				+ NIFACES * IF_QUEUESIZE + 1;
	netbufpool = mkbufpool(PACKLEN, nbufs);

	if (netbufpool == SYSERR) {
		kprintf("Cannot allocate network buffer pool\n");
		kill(getpid());
	}

	pkt = (struct netpacket *)getbuf(netbufpool);
	while(1) {
	    	retval = read(ETHER0, (char *)pkt, PACKLEN);
	    	if (retval == SYSERR) {
			panic("Ethernet read error");
	    	}

		/* Demultiplex on MAC address */

		for (iface=0; iface<=NIFACES; iface++) {
		    ifptr = &if_tab[iface];
		    if (ifptr->if_state != IF_UP) {
			continue;
		    }
		    if ( (memcmp(ifptr->if_macucast, pkt->net_ethdst,
				ETH_ADDR_LEN) == 0) ||
			 (memcmp(ifptr->if_macbcast, pkt->net_ethdst,
				ETH_ADDR_LEN) == 0)) {

			/* packet goes to this interface */

			/* Check interface queue; drop packet if full	*/
									
			if (semcount(ifptr->if_sem) >= IF_QUEUESIZE) {
				kprintf("rawin: queue overflow on %d\n", iface);
				break;
			}

			/* Enqueue packet and signal semaphore */

			ifptr->if_queue[ifptr->if_tail++] = pkt;
			if (ifptr->if_tail >= IF_QUEUESIZE) {
				ifptr->if_tail = 0;
			}
			signal(ifptr->if_sem);

			/* Obtain a buffer for the next packet */

			pkt = (struct netpacket *)getbuf(netbufpool);
			break;
		    }
		}
	}
}
#endif
/*------------------------------------------------------------------------
 * eth_hton - convert Ethernet type field to network byte order
 *------------------------------------------------------------------------
 */
void 	eth_hton(
	  struct netpacket *pktptr
	)
{
	pktptr->net_ethtype = htons(pktptr->net_ethtype);
}


/*------------------------------------------------------------------------
 * eth_ntoh - convert Ethernet type field to host byte order
 *------------------------------------------------------------------------
 */
void 	eth_ntoh(
	  struct netpacket *pktptr
	)
{
	pktptr->net_ethtype = ntohs(pktptr->net_ethtype);
}
