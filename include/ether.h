/* ether.h */

/* Ethernet packet format:

 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |  Dest. MAC (6)  |  Src. MAC (6)   |Type (2)|      Data (46-1500)...   |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define	ETH_ADDR_LEN	6	/* Length of Ethernet (MAC) address	*/
typedef	unsigned char	Eaddr[ETH_ADDR_LEN];/* a physical Ethernet address*/

/* Ethernet packet header */

struct	etherPkt {
	byte	dst[ETH_ADDR_LEN];	/* Destination Mac address	*/
	byte	src[ETH_ADDR_LEN];	/* Source Mac address		*/
	uint16	type;			/* Ether type field		*/
	byte	data[1];		/* Packet payload		*/
};

#define	ETH_HDR_LEN		14	/* Length of Ethernet packet 	*/
					/*  header			*/

/* Ethernet DMA buffer sizes */

#define	ETH_MTU			1500	/* Maximum transmission unit	*/
#define	ETH_VLAN_LEN		4	/* Length of Ethernet vlan tag	*/
#define ETH_CRC_LEN		4	/* Length of CRC on Ethernet 	*/
					/*  frame			*/

#define	ETH_MAX_PKT_LEN	( ETH_HDR_LEN + ETH_VLAN_LEN + ETH_MTU )

#define	ETH_BUF_SIZE		2048	/* A multiple of 16 greater 	*/
					/* 	than the max packet 	*/
					/*  	size(cache alignment)	*/

/* State of the Ethernet interface */

#define	ETH_STATE_FREE		0	/* control block is unused 	*/
#define	ETH_STATE_DOWN		1	/* interface is currently  	*/
					/* 	inactive 		*/
#define	ETH_STATE_UP		2	/* interface is currently active*/

/* Ethernet device control functions */

#define	ETH_CTRL_GET_MAC     	1 	/* Get the MAC for this device	*/
#define ETH_CTRL_ADD_MCAST	2	/* Add a multicast address	*/
#define ETH_CTRL_REMOVE_MCAST	3	/* Remove a multicast address	*/

/* Ethernet multicast */

#define ETH_NUM_MCAST		32     /* Max number of multicast addresses*/

/* Ehternet NIC type */

#define ETH_TYPE_3C905C 	1
#define ETH_TYPE_E1000E 	2
#define ETH_TYPE_QUARK_ETH 	3

struct	ether	{
	byte	state; 		/* ETH_STATE_... as defined above 	*/
	struct	dentry	*phy;	/* physical eth device for Tx DMA 	*/
	byte 	type; 		/* NIC type_... as defined above 	*/

	/* Pointers to associated structures */

	struct	dentry	*dev;	/* address in device switch table	*/
	void	*csr;		/* addr.of control and status regs.	*/
	uint32	pcidev;		/* PCI device number			*/
	uint32	iobase;		/* I/O base from config			*/
	uint32  flashbase;      /* flash base from config	       	*/
    	uint32	membase; 	/* memory base for device from config	*/

	void    *rxRing;	/* ptr to array of recv ring descriptors*/
	void    *rxBufs; 	/* ptr to Rx packet buffers in memory	*/
	uint32	rxHead;		/* Index of current head of Rx ring	*/
	uint32	rxTail;		/* Index of current tail of Rx ring	*/
	uint32	rxRingSize;	/* size of Rx ring descriptor array	*/
	uint32	rxIrq;		/* Count of Rx interrupt requests       */

	void    *txRing; 	/* ptr to array of xmit ring descriptors*/
	void    *txBufs; 	/* ptr to Tx packet buffers in memory	*/
	uint32	txHead;		/* Index of current head of Tx ring	*/
	uint32	txTail;		/* Index of current tail of Tx ring	*/
	uint32	txRingSize;	/* size of Tx ring descriptor array	*/
	uint32	txIrq;		/* Count of Tx interrupt requests       */

	uint8	devAddress[ETH_ADDR_LEN];/* MAC address 		*/

	uint8	addrLen;	/* Hardware address length	      	*/
	uint16	mtu;	    	/* Maximum transmission unit (payload)  */

	uint32	errors;		/* Number of Ethernet errors 		*/
	sid32	isem;		/* Semaphore for Ethernet input		*/
	sid32	osem; 		/* Semaphore for Ethernet output	*/
	uint16	istart;		/* Index of next packet in the ring     */

	int16	inPool;		/* Buffer pool ID for input buffers 	*/
	int16	outPool;	/* Buffer pool ID for output buffers	*/

	int16 	proms; 		/* nonzero => promiscuous mode 		*/
    
	int16 	ed_mcset;       /* nonzero => multicast reception set   */
	int16 	ed_mcc;	 	/* count of multicast addresses		*/
    	Eaddr   ed_mca[ETH_NUM_MCAST];/* array of multicast addrs 	*/

	/* Late binding operations */

	void 	(*ethInit)(struct ether *ethptr);
	status 	(*ethOpen)(struct ether *ethptr);
	status 	(*ethClose)(struct ether *ethptr);
	devcall (*ethRead)(struct ether *ethptr, void *buf, uint32 len);
	devcall (*ethWrite)(struct ether *ethptr, void *buf, uint32 len);
	devcall (*ethControl)(struct ether *ethptr, int32 func, 
			int32 arg1, int32 arg2);
	interrupt (*ethInterrupt)(struct ether *ethptr);
};

extern	struct	ether	ethertab[];	/* array of control blocks      */

int32	colon2mac(char *, byte *);
int32	allocRxBuffer(struct ether *, int32);
int32	waitOnBit(volatile uint32 *, uint32, const int32, int32);

typedef	uint32	IPaddr;
