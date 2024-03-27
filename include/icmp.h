/* icmp.h - definintions for the Internet Control Message Protocol */

#define	ICMP_SLOTS	10		/* Num. of open ICMP endpoints	*/
#define	ICMP_QSIZ	8		/* Incoming packets per slot	*/

/* Constants for the state of an entry */

#define	ICMP_FREE	0		/* Entry is unused		*/
#define	ICMP_USED	1		/* Entry is being used		*/
#define	ICMP_RECV	2		/* Entry has a process waiting	*/

#define ICMP_HDR_LEN	8		/* Bytes in an ICMP header	*/

/* ICMP message types for ping */

#define	ICMP_ECHOREPLY	0		/* ICMP Echo Reply message	*/
#define ICMP_ECHOREQST	8		/* ICMP Echo Request message	*/

/* Table of processes that are waiting for ping replies */

struct	icmpentry {			/* Entry in the ICMP table	*/
	int32	icstate;		/* State of entry: free/used	*/
	uint32	icremip;		/* Remote IP address		*/
	int32	ichead;			/* Index of next packet to read	*/
	int32	ictail;			/* Index of next slot to insert	*/
	int32	iccount;		/* Count of packets enqueued	*/
	pid32	icpid;			/* ID of waiting process	*/
	struct	netpacket *icqueue[ICMP_QSIZ];/* Circular packet queue	*/
};

extern	struct	icmpentry icmptab[];	/* Table of UDP endpoints	*/
