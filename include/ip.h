/* ip.h  -  constants related to IPv4 */

#define	IP_BCAST	0xffffffff	/* IP local broadcast address	*/
#define	IP_THIS		0xffffffff	/* "this host" src IP address	*/
#define	IP_ALLZEROS	0x00000000	/* The all-zeros IP address     */

#define	IP_ICMP		1		/* ICMP protocol type for IP 	*/
#define	IP_UDP		17		/* UDP protocol type for IP 	*/

#define	IP_ASIZE	4		/* bytes in an IP address	*/
#define	IP_HDR_LEN	20		/* bytes in an IP header	*/
#define IP_VH		0x45 		/* IP version and hdr length 	*/

#define	IP_OQSIZ	8		/* size of IP output queue	*/

/* Queue of outgoing IP packets waiting for ipout process */

struct	iqentry	{
	int32	iqhead;			/* index of next packet to send	*/
	int32	iqtail;			/* index of next free slot	*/
	sid32	iqsem;			/* semaphore that counts pkts	*/
	struct	netpacket *iqbuf[IP_OQSIZ];/* circular packet queue	*/
};

extern	struct	iqentry	ipoqueue;	/* network output queue		*/
