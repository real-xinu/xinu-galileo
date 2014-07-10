/* nat.c - nat_init, nat_in, nat_out, nat_udp_in, nat_udp_out,		*/
/*	nat_genuport, nat_icmp_in, nat_icmp_out				*/

#include <xinu.h>

struct	nuentry	nat_udp[NATUDPSIZ];
struct	nientry	nat_icmp[NATICMPSIZ];
extern	uint32	clktime;

/*------------------------------------------------------------------------
 * nat_init - initialize the NAT mapping table
 *------------------------------------------------------------------------
 */
void	nat_init(void)
{
	int32	i;			/* index into NAT table		*/

	memset((char *)&nat_udp, NULLCH, sizeof(nat_udp));
	for (i=0; i<NATUDPSIZ; i++) {
		nat_udp[i].nu_state = NAT_FREE;
	}

	memset((char *)&nat_icmp, NULLCH, sizeof(nat_icmp));
	for (i=0; i<NATICMPSIZ; i++) {
		nat_icmp[i].ni_state = NAT_FREE;
	}
}

/*------------------------------------------------------------------------
 * nat_in - perform NAT mapping on IP packets coming in from the Internet
 *------------------------------------------------------------------------
 */
void	nat_in(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)
{
	switch (pktptr->net_ipproto) {

	    case IP_UDP:
		nat_in_udp(pktptr);
		return;

	    case IP_ICMP:
		nat_in_icmp(pktptr);
		return;

	default:
		ip_local(pktptr);
		return;
	}
}

/*------------------------------------------------------------------------
 * nat_out - perform NAT mapping on IP packets going out to the Internet
 *------------------------------------------------------------------------
 */

void	nat_out(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)
{
	switch (pktptr->net_ipproto) {

	    case IP_UDP:
		nat_out_udp(pktptr);
		return;

	    case IP_ICMP:
		nat_out_icmp(pktptr);
		return;

	default:
		freebuf((char *)pktptr);
		return;
	}
}


/*------------------------------------------------------------------------
 * nat_in_udp - perform NAT on UDP packet coming in from the Internet
 *		IP in host byte order but UDP still in network byte order
 *------------------------------------------------------------------------
 */

void	nat_in_udp(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)
{
	int32	slot;			/* index into NAT table		*/
	struct	nuentry	*nptr;		/* ptr to entry in table	*/
	uint32	currtime;		/* Current time stamp relative	*/
					/*   to system boot		*/

	/* Obtain timestamp for timeout comparison */

	currtime = clktime;

	/* Search NAT table */

	for (slot=0; slot<NATUDPSIZ; slot++) {
		nptr = &nat_udp[slot];
		if (nptr->nu_state == NAT_FREE) {
			continue;
		}

		/* Check whether IP source, IP destination, UDP source	*/
		/*	port, and UDP destination port match		*/

		if ( (pktptr->net_ipsrc == nptr->nu_destip) &&
		     (pktptr->net_ipdst == nptr->nu_pubip) &&
		     (pktptr->net_udpsport == nptr->nu_destpt) &&
		     (pktptr->net_udpdport == nptr->nu_pubpt) ) {
			nptr->nu_time = currtime;
			break;
		}

		/* If timeout exceeded, mark entry free */

		if ((currtime - nptr->nu_time) > NAT_TIMEOUT) {
			nptr->nu_state = NAT_FREE;
		}
	}

	/* If no match found in NAT table, send to local stack */

	if (slot >= NATUDPSIZ) {
		ip_local(pktptr);
		return;
	}

	/* Rewrite packet to make IP destination and UDP dest port are	*/
	/*	the ones expected by the othernet host			*/

	pktptr->net_ipdst = nptr->nu_privip;
	pktptr->net_udpdport = nptr->nu_privpt;
	pktptr->net_udpcksum = 0;
	
	/* Enqueue for transmission over the interface that was		*/
	/*	recorded in the table entry				*/

	pktptr->net_iface = nptr->nu_iface;
	ip_enqueue(pktptr);
	return;
}


/*------------------------------------------------------------------------
 * nat_out_udp - perform NAT on UDP packet going out to the Internet
 *		IP in host byte order but UDP still in network byte order
 *------------------------------------------------------------------------
 */

void	nat_out_udp(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)
{
	int32	slot;			/* index into NAT table		*/
	struct	nuentry	*nptr;		/* ptr to entry in table	*/
	int32	freeslot;		/* Free slot in table		*/
	uint32	currtime;		/* Current time stamp relative	*/
					/*   to system boot		*/

	/* Obtain current time for timeout comparison */

	currtime = clktime;

	/* Search NAT table and record first free slot */

	freeslot = -1;
	for (slot=0; slot<NATUDPSIZ; slot++) {
		nptr = &nat_udp[slot];
		if (nptr->nu_state == NAT_FREE) {
			if (freeslot < 0) {
				freeslot = slot;
			}
			continue;
		}

		/* Check whether IP source, IP destination, UDP source	*/
		/*	port, and UDP destination port match		*/

		if ( (pktptr->net_ipsrc == nptr->nu_privip) &&
		     (pktptr->net_ipdst == nptr->nu_destip) &&
		     (pktptr->net_udpsport == nptr->nu_privpt) &&
		     (pktptr->net_udpdport == nptr->nu_destpt) ) {
			nptr->nu_time = currtime;
			break;
		}

		/* If timeout exceeded, mark entry free */

		if ((currtime - nptr->nu_time) > NAT_TIMEOUT) {
			nptr->nu_state = NAT_FREE;
			if (freeslot < 0) {
				freeslot = slot;
			}
			continue;
		}
	}

	if (slot >= NATUDPSIZ) {	/* not currently in table */
		if (freeslot < 0) {
			panic("NAT table overflow in nat_out_udp");
		}
		slot = freeslot;
		nptr = &nat_udp[slot];
		nptr->nu_time = currtime;
		nptr->nu_iface = pktptr->net_iface;
		nptr->nu_destip = pktptr->net_ipdst;
		nptr->nu_destpt = pktptr->net_udpdport;
		nptr->nu_pubip = if_tab[0].if_ipucast;
		nptr->nu_pubpt = nat_genuport(nptr);
		nptr->nu_privip = pktptr->net_ipsrc;
		nptr->nu_privpt = pktptr->net_udpsport;
		nptr->nu_state = NAT_USED;
	}

	/* Rewrite packet to make IP source address and UDP source port	*/
	/*	valid for the global Internet				*/

	pktptr->net_ipsrc = nptr->nu_pubip;
	pktptr->net_udpsport = nptr->nu_pubpt;
	pktptr->net_udpcksum = 0;
	
	/* Enqueue for transmission on the Internet (interface 0)	*/

	pktptr->net_iface = 0;
	ip_enqueue(pktptr);
	return;
}


/*------------------------------------------------------------------------
 * nat_genuport - generate a unique public UDP port number for NAT
 *------------------------------------------------------------------------
 */

uint16	nat_genuport(
	  struct nuentry *nptr		/* ptr to the NAT table entry	*/
	)
{
	static	uint16	port = 40000;	/* port number to try		*/
	int32	slot;			/* index into NAT table		*/
	bool8	done;			/* has a valid port been found?	*/

	/* Search NAT table */

	done = FALSE;
	port++;
	while (!done) {
		for (slot=0; slot<NATUDPSIZ; slot++) {
			nptr = &nat_udp[slot];
			if (nptr->nu_state == NAT_FREE) {
				continue;
			}

			/* If port number used, increment and try again	*/

			if (port == nptr->nu_pubpt) {
				port++;
				if (port >= 50000) {
					port = 40000;
				}
				break;
			}
		}
		if (slot >= NATUDPSIZ) {  /* No match in table */
			done = TRUE;
		}
	}
	return port;
}


/*------------------------------------------------------------------------
 * nat_in_icmp - perform NAT on an ICMP ping reply coming in from the
 *		    Internet if it matches a previously-sent request
 *------------------------------------------------------------------------
 */

void	nat_in_icmp(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)

{
	int32	slot;			/* index into NAT table		*/
	struct	nientry	*nptr;		/* ptr to entry in table	*/

	/* Only perform NAT for ICMP echo reply packets */

	if (pktptr->net_ictype != ICMP_ECHOREPLY) {
		ip_local(pktptr);
		return;
	}

	/* This is a reply - use ID as slot in table */

	/* First check the native range */

	slot = pktptr->net_icident;

	if ( (slot >= 0) && (slot <ICMP_SLOTS) ) {
		ip_local(pktptr);
	}

	/* Convert to a slot in the NAT table */

	slot = nat_icmp_pub2slot(slot);
	if ( (slot < 0) || (slot >= NATICMPSIZ) ) {
		freebuf((char *)pktptr);
		return;
	}

	/* Find correct entry in table */

	nptr = &nat_icmp[slot];

	/* Verify that IP source matches table entry */

	if (pktptr->net_ipsrc != nptr->ni_destip) {
		freebuf((char *)pktptr);
		return;
	}

	/* Rewrite packet to make IP destination and ICMP ID the ones	*/
	/*	expected by the othernet host				*/

	pktptr->net_ipdst = nptr->ni_privip;
	pktptr->net_icident = nptr->ni_privid;
	pktptr->net_iccksum = 0;

	/* Place in IP output queue for transmission on the interface	*/
	/*	that has been recorded in the table entry		*/

	pktptr->net_iface = nptr->ni_iface;
	ip_enqueue(pktptr);
	return;
}


/*------------------------------------------------------------------------
 * nat_out_icmp - perform NAT on ICMP packet going out to the Internet
 *------------------------------------------------------------------------
 */

void	nat_out_icmp(
	  struct netpacket *pktptr	/* ptr to the packet to process	*/
	)
{
	int32	slot;			/* index into NAT table		*/
	struct	nientry	*nptr;		/* ptr to entry in table	*/
	int32	freeslot;		/* Free slot in table		*/
	uint32	currtime;		/* Current time stamp relative	*/
					/*   to system boot		*/

	/* Only perform outgoing NAT for ICMP echo request packets */

	if (pktptr->net_ictype != ICMP_ECHOREQST) {
		freebuf((char *)pktptr);
		return;
	}

	/* Obtain current time for timeout comparison */

	currtime = clktime;

	freeslot = -1;

	/* Search NAT table */

	for (slot=0; slot<NATICMPSIZ; slot++) {
		nptr = &nat_icmp[slot];
		if (nptr->ni_state == NAT_FREE) {
			if (freeslot < 0) {
				freeslot = slot;
			}
			continue;
		}

		/* If timeout exceeded, mark entry free */

		if ((currtime - nptr->ni_time) > NAT_TIMEOUT) {
			nptr->ni_state = NAT_FREE;
			if (freeslot < 0) {
				freeslot = slot;
			}
			continue;
		}

		/* Check whether IP source, IP destination, and		*/
		/*	ICMP match entry in table			*/

		if ( (pktptr->net_ipsrc == nptr->ni_privip) &&
		     (pktptr->net_ipdst == nptr->ni_destip) &&
		     (pktptr->net_icident == nptr->ni_privid) ) {
			break;
		}
	}

	if (slot >= NATICMPSIZ) {
		if (freeslot < 0) {
			panic("NAT table overflow in nat_out_icmp");
		}
		slot = freeslot;
		nptr = &nat_icmp[slot];
		nptr->ni_time = currtime;
		nptr->ni_iface = pktptr->net_iface;
		nptr->ni_destip = pktptr->net_ipdst;
		nptr->ni_privip = pktptr->net_ipsrc;
		nptr->ni_privid = pktptr->net_icident;
	}

	/* Rewrite packet to make IP source address and ICMP ID	valid	*/
	/*	for the global Internet					*/

	pktptr->net_ipsrc = if_tab[0].if_ipucast;
	slot = nat_icmp_slot2pub(slot);
	pktptr->net_icident = 0xffff & slot;
	pktptr->net_iccksum = 0;

	/* Enqueue for transmission on the Internet interface (0)	*/

	pktptr->net_iface = 0;
	ip_enqueue(pktptr);
	return;
}
