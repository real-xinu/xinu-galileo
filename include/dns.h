/* dns.h */

/* Format of a DNS Query/Response packet */
struct	dnspkt {
	uint16	id;		/* DNS Query ID		*/
	struct	{
		byte	rd:1;	/* Recursion Desired	*/
		byte	tc:1;	/* Truncation		*/
		byte	aa:1;	/* Authoritative Answer	*/
		byte	opcode:4;/* Operation Code	*/
		byte	qr:1;	/* Query=0, Response=1	*/
	};
	struct	{
		byte	rcode:4;/* Response Code	*/
		byte	z:3;	/* Reserved, must be 0	*/
		byte	ra:1;	/* Recursion Available	*/
	};
	uint16	qdcount;	/* No. of Questions	*/
	uint16	ancount;	/* No. of RRs in Answer	*/
	uint16	nscount;	/* No of NS RRs		*/
	uint16	arcount;	/* No. of RRs in Addntl	*/
	char	data[500];	/* DNS Data		*/
};

/* QType Values */

#define	DNS_QT_A	1	/* DNS Address Type	*/
#define DNS_QT_NS	2	/* DNS Name Server Type	*/

/* QClass values */

#define	DNS_QC_IN	1	/* DNS QClass Internet	*/

/* Format of a DNS Question */
struct	dns_q {
	char	*qname;		/* Domain Name		*/
	uint16	*qtype;		/* Question Type	*/
	uint16	*qclass;	/* Question Class	*/
};

/* Format of a DNS Resource Record */
struct	dns_rr {
	char	*rname;		/* Domain Name		*/
	uint16	*rtype;		/* RR Type		*/
	uint16	*rclass;	/* RR Class		*/
	uint32	*ttl;		/* RR Time-to-Live	*/
	uint16	*rdlen;		/* RR RD Length		*/
	char	*rdata;		/* RR Data		*/
};
