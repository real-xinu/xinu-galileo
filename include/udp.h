/* udp.h - declarations pertaining to User Datagram Protocol (UDP) */

#define	UDP_SLOTS	6 		/* num. of open UDP endpoints 	*/
#define	UDP_QSIZ	8		/* packets enqueued per endpoint*/

#define	UDP_DHCP_CPORT	68		/* port number for DHCP client	*/
#define	UDP_DHCP_SPORT	67		/* port number for DHCP server	*/

/* Constants for the state of an entry */

#define	UDP_FREE	0 		/* entry is unused		*/
#define	UDP_USED	1		/* entry is being used		*/
#define	UDP_RECV	2		/* entry has a process waiting	*/

#define	UDP_ANYIF	-2		/* Register an endpoint for any	*/
					/*   interface on the machine	*/

#define UDP_HDR_LEN	8		/* bytes in a UDP header	*/

struct	udpentry {			/* entry in the UDP endpoint tbl*/
	int32	udstate;		/* state of entry: free/used	*/
	uint32	udremip;		/* remote IP address (zero	*/
					/*   means "don't care")	*/
	uint16	udremport;		/* remote protocol port number	*/
	uint16	udlocport;		/* local protocol port number	*/
	int32	udhead;			/* index of next packet to read	*/
	int32	udtail;			/* index of next slot to insert	*/
	int32	udcount;		/* count of packets enqueued	*/
	pid32	udpid;			/* ID of waiting process	*/
	struct	netpacket *udqueue[UDP_QSIZ];/* circular packet queue	*/
};

extern	struct	udpentry udptab[];
