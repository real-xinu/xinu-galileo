/* dns.c - dns_qa, dns_bldq, dns_geta, dns_getrname */

#include <xinu.h>
#include <string.h>
#include <dns.h>

uint32	nsaddr;

uint32	dns_bldq(char *, char *);
uint32	dns_geta(char *, struct dnspkt *);
uint32	dns_getrname(char *, char *, char *);

/*------------------------------------------------------------------------
 * dns_qa - Send a DNS Address Query and wait for the Response
 *------------------------------------------------------------------------
 */
uint32	dns_qa	(
	char	*dname	/* Domain name to be resolved	*/
	)
{
	struct	dnspkt	*qpkt;	/* Query Packet	*/
	struct	dnspkt	*rpkt;	/* Response Pkt	*/
	uint32	qlen;		/* Query length	*/
	uid32	slot;		/* UDP Slot	*/
	int32	rlen;		/* Response len	*/
	uint32	ipaddr;		/* IP address	*/
	int32	i;

	/* Check if we have a valid name pointer */

	if(dname == NULL) {
		return (uint32)SYSERR;
	}

	/* Register a UDP Slot */

	slot = udp_register(nsaddr, 53, 12345);
	if(slot == SYSERR) {
		return (uint32)SYSERR;
	}

	/* Allocate memory for Query and Response messages */

	qpkt = (struct dnspkt *)getmem(sizeof(struct dnspkt));
	if((int32)qpkt == SYSERR) {
		udp_release(slot);
		return (uint32)SYSERR;
	}

	rpkt = (struct dnspkt *)getmem(sizeof(struct dnspkt));
	if((int32)rpkt == SYSERR) {
		udp_release(slot);
		freemem((char*)qpkt, sizeof(struct dnspkt));
		return (uint32)SYSERR;
	}

	/* Build the Query message */

	memset((char *)qpkt, 0, sizeof(struct dnspkt));

	qpkt->id = currpid;
	qpkt->rd = 1;
	qpkt->qdcount = htons(1);

	//kprintf("building question\n");
	qlen = dns_bldq(dname, qpkt->data);
	/*for(i = 0; i < qlen; i++) {
		kprintf("%x ", qpkt->data[i]);
	}*/

	ipaddr = (uint32)SYSERR;
	for(i = 0; i < 3; i++) {

		/* Send the Query message */

		udp_send(slot, (char*)qpkt, 12 + qlen);

		/* Wait for the response */

		rlen = udp_recv(slot, (char*)rpkt, 512, 3000);
		if(rlen == SYSERR) {
			continue;
		}
		else if(rlen == TIMEOUT) {
			continue;
		}
		else {
			/*kprintf("Response header: \n");
			uint16 *ptr = (uint16 *)rpkt;
			for(i = 0; i < 4; i++) {
				kprintf("%04X\n", *ptr);
				ptr++;
			}*/
			ipaddr = dns_geta(dname, rpkt);
		}
	}

	udp_release(slot);
	freemem((char *)qpkt, sizeof(struct dnspkt));
	freemem((char *)rpkt, sizeof(struct dnspkt));
	return ntohl(ipaddr);
}

/*------------------------------------------------------------------------
 * dns_bldq - Build a DNS Question and return the length
 *------------------------------------------------------------------------
 */
uint32	dns_bldq (
	char	*dname,	/* Domain Name	*/
	char	*data	/* Dest pointer	*/
	)
{
	uint32	qlen;	/* Length of Question	*/
	uint32	dlen;	/* Length of domain name*/
	byte	*llptr;	/* Label length pointer	*/
	int32	i;

	/* Get length of the domain name */

	dlen = strlen(dname);

	llptr = (byte*)(data++);
	*llptr = 0;
	qlen = 1;
	for(i = 0; i < dlen; i++) {

		if(qlen >= 512) {
			return (uint32)SYSERR;
		}
		if(dname[i] != '.') {
			*data++ = dname[i];
			*llptr = *llptr + 1;
		}
		else {
			llptr = (byte*)(data++);
			*llptr = 0;
		}
		qlen++;
	}

	*data++ = 0;
	qlen++;

	/* Qtype = Address */
	*((uint16 *)data) = htons(DNS_QT_A);
	data += 2;
	qlen += 2;

	/* QClass = Internet */
	*((uint16 *)data) = htons(DNS_QC_IN);
	qlen += 2;

	return qlen;
}

/*------------------------------------------------------------------------
 * dns_geta - returns the first IP address in the Answer Section
 *------------------------------------------------------------------------
 */
uint32	dns_geta (
	char	*dname,		/* Domain Name		*/
	struct	dnspkt *rpkt	/* Response packet	*/
	)
{
	uint16	qdcount;	/* No. of Questions	*/
	uint16	ancount;	/* No. of Answers	*/
	char	*dptr;		/* Data pointer		*/
	int32	i;

	qdcount = ntohs(rpkt->qdcount);
	dptr = rpkt->data;

	//kprintf("skipping %d questions\n", qdcount);
	for(i = 0; i < qdcount; i++) {

		byte	llen;

		llen = *((byte *)dptr);

		while(llen != 0) {

			if(llen > 63) {
				dptr += 2;
				break;
			}

			dptr += (1 + llen);
			llen = *((byte *)dptr);
		}

		if(llen == 0) {
			dptr += 1;
		}
		dptr += (2 + 2);
	}

	ancount = ntohs(rpkt->ancount);
	//kprintf("we have %d answers\n", ancount);

	for(i = 0; i < ancount; i++) {

		char	rname[256];
		uint32	dlen;

		dlen = dns_getrname((char *)rpkt, dptr, rname);
		//kprintf("name %s\n", rname);
		dptr += dlen;

		if(strncmp(rname, dname, strlen(dname)) == 0) {
			if(ntohs(*((uint16 *)dptr)) == DNS_QT_A) {
				dptr += 10;
				return *((uint32 *)dptr);
			}
		}

		dptr += 8;
		dptr += (2 + ntohs(*((uint16 *)dptr)));
	}

	return (uint32)SYSERR;
}

/*------------------------------------------------------------------------
 * dns_getrname - Create a C string from Resource Record
 *------------------------------------------------------------------------
 */
uint32	dns_getrname (
	char	*sop,	/* Start of Packet	*/
	char	*son,	/* Start of Name	*/
	char	*dst	/* Destination address	*/
	)
{
	byte	llen;
	char	*sson = son;
	int32	i;

	llen = *((byte *)son++);

	while(llen != 0) {

		//kprintf("llen %d\n", llen);
		if(llen <= 63) {
			for(i = 0; i < llen; i++) {
				*dst++ = *son++;
			}
			*dst++ = '.';
			llen = *((byte *)son++);
		}
		else {
			uint16	offset;

			son--;
			(*son) = (*son) & 0x3f;

			offset = (uint16)(*son++) * 256;
			offset += (uint16)(*son++);

			dns_getrname(sop, sop+offset, dst);

			return (son-sson);

		}
	}

	*dst = NULLCH;

	return (uint32)(son-sson);
}
