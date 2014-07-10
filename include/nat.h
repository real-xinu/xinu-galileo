/* nat.h - definitions related to  NAT tables */

#define	NATUDPSIZ	40		/* number of entries in UDP	*/
					/*   NAT table			*/

#define	NATICMPSIZ	40		/* number of entries in ICMP	*/
					/*   NAT table			*/

/* Constants for state of entry in NAT table */

#define	NAT_FREE	0		/* Entry is unused		*/
#define	NAT_USED	1		/* Entry is in use		*/

/* Translations between slot in the NAT table for ICMP and the ID used	*/
/*	in ping packets.  Basically, native ICMP uses numbers 0 through	*/
/*	ICMP_SLOTS-1 as the ID, and NAT uses values ICMP_SLOTS through	*/
/*	ICMP_SLOTS+NAT_ICMPSIZ-1.  The following in-line functions	*/
/*	convert between a public ICMP ID value and a NAT table slot.	*/

#define	nat_icmp_slot2pub(x)	((x)+ICMP_SLOTS);
#define	nat_icmp_pub2slot(x)	((x)-ICMP_SLOTS);

/* Maximum time before unused entry is considered expired in seconds	*/

#define	NAT_TIMEOUT	300		/* 5 minutes			*/

/* Definintion of a UDP NAT table entry */

struct	nuentry	{
	int32	nu_state;		/* State of entry: free/used	*/
	int32	nu_iface;		/* Interface for this entry	*/
	uint32	nu_destip;		/* destination IP address	*/
	uint32	nu_destpt;		/* destination UDP port		*/
	uint32	nu_pubip;		/* public IP source addr. used	*/
	uint16	nu_pubpt;		/* public UDP source port used	*/
	uint32	nu_privip;		/* private IP address		*/
	uint16	nu_privpt;		/* private UDP port		*/
	uint32	nu_time;		/* time entry was last used	*/
};

extern	struct	nuentry	nat_udp[];

/* Definintion of an ICMP NAT table entry */

struct	nientry	{
	int32	ni_state;		/* State of entry: free/used	*/
	int32	ni_iface;		/* Interface for this entry	*/
	uint32	ni_destip;		/* destination IP address	*/
	uint32	ni_privip;		/* private IP address		*/
	uint16	ni_privid;		/* private ICMP ID		*/
	uint32	ni_time;		/* time entry was last used	*/
};

extern	struct	nientry	nat_icmp[];
