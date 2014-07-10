/* netiface.h - definintions for network interfaces used by IP */

#ifndef Noth
#define	Noth		0
#endif
#define	NIFACES	3			/* number of intefaces (the	*/
					/*   the external Internet	*/
					/*   plus two "backend" nets.	*/
#define	IF_UP		1	/* Interface is currently on line	*/
#define	IF_DOWN		0	/* Interface is currently offline	*/
#define	IF_QUEUESIZE	20	/* Size of packet queue for interface	*/	

/* Network interface */

struct	ifentry	{
	bool8	if_state;	/* interface is either up or down	*/
	did32	if_dev;		/* Device ID for layer2 device used	*/
				/*    with this interface		*/
	byte	if_macucast[ETH_ADDR_LEN]; /* MAC unicast address	*/
	byte	if_macbcast[ETH_ADDR_LEN]; /* MAC broadcast address	*/
	bool8	if_ipvalid;	/* Are IP fields valid yet?		*/
	uint32	if_ipucast;	/* IP unicast address			*/
	uint32	if_ipbcast;	/* IP net broadcast address		*/
	uint32	if_ipmask;	/* IP address mask			*/
	uint32	if_ipprefix;	/* IP prefix for the network		*/
	uint32	if_iprouter;	/* IP address of default router (valid	*/
				/*    only on interface 0 - Ethernet)	*/
	sid32	if_sem;		/* semaphore counts incoming packets	*/
				/*    in the queue			*/
	struct	netpacket *if_queue[IF_QUEUESIZE]; /* queue to hold	*/
				/*  incoming packets for this interface	*/
	int32	if_head;	/* next entry in packet queue to remove	*/
	int32	if_tail;	/* next slot in packet queue to insert	*/
	struct	arpentry if_arptab[ARP_SIZ];	/* ARP cache for the	*/
};						/*   interface		*/

extern	struct	ifentry	if_tab[];

extern	int32	ifprime;	/* Primary interface.  For a host, the	*/
				/*   only interface that's up, for a	*/
				/*   router, interface 0.  -1 means	*/
				/*   no network is active.		*/
extern	bool8	host;		/* TRUE if running a host		*/
extern	int32	bingid;		/* User's bing ID			*/
