/* ether.h */

/* Ethernet packet format:

 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |  Dest. MAC (6)  |  Src. MAC (6)   |Type (2)|      Data (46-1500)...   |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define	ETH_ADDR_LEN	6		/* Len. of Ethernet (MAC) addr.	*/
typedef	unsigned char	Eaddr[ETH_ADDR_LEN];/* Physical Ethernet address*/

/* Ethernet packet header */

struct	etherPkt {
	byte	dst[ETH_ADDR_LEN];	/* Destination Mac address	*/
	byte	src[ETH_ADDR_LEN];	/* Source Mac address		*/
	uint16	type;			/* Ether type field		*/
	byte	data[1];		/* Packet payload		*/
};

#define	ETH_HDR_LEN		14	/* Length of Ethernet packet 	*/
					/*   header			*/

/* Ethernet DMA buffer sizes */

#define	ETH_MTU			1500	/* Maximum transmission unit	*/
#define	ETH_VLAN_LEN		4	/* Length of Ethernet vlan tag	*/
#define ETH_CRC_LEN		4	/* Length of CRC on Ethernet 	*/
					/*   frame			*/

#define	ETH_MAX_PKT_LEN	( ETH_HDR_LEN + ETH_VLAN_LEN + ETH_MTU )

#define	ETH_BUF_SIZE		1522	/* 1500 MTU + 14 ETH header + 4	*/
					/* bytes optional VLAN Tagging  */
					/* + 4 Bytes CRC		*/

/* State of the Ethernet interface */

#define	ETH_STATE_FREE		0	/* Control block is unused 	*/
#define	ETH_STATE_DOWN		1	/* Interface is inactive	*/
#define	ETH_STATE_UP		2	/* Interface is currently active*/

/* Ethernet device control functions */

#define	ETH_CTRL_GET_MAC     	1 	/* Get the MAC for this device	*/
#define ETH_CTRL_ADD_MCAST	2	/* Add a multicast address	*/
#define ETH_CTRL_REMOVE_MCAST	3	/* Remove a multicast address	*/

/* Ethernet multicast */

#define ETH_NUM_MCAST		32	/* Max multicast addresses	*/

/* Ehternet NIC type */

#define ETH_TYPE_3C905C 	1	/* 3COM 905C			*/
#define ETH_TYPE_E1000E 	2	/* Intel E1000E			*/
#define ETH_TYPE_QUARK_ETH 	3	/* Ethernet on Quark board	*/

/* Control block for Ethernet device */

struct	ethcblk	{
	byte	state; 		/* ETH_STATE_... as defined above 	*/
	struct	dentry	*phy;	/* physical eth device for Tx DMA 	*/
	byte 	type; 		/* NIC_TYPE_... as defined above 	*/

	/* Pointers to associated structures */

	struct	dentry	*dev;	/* Address in device switch table	*/
	void	*csr;		/* Control and status regsiter address	*/
	uint32	pcidev;		/* PCI device number			*/
	uint32	iobase;		/* I/O base from config			*/
	uint32  flashbase;      /* Flash base from config	       	*/
    	uint32	membase; 	/* Memory base for device from config	*/

	void    *rxRing;	/* Ptr to array of recv ring descriptors*/
	void    *rxBufs; 	/* Ptr to Rx packet buffers in memory	*/
	uint32	rxHead;		/* Index of current head of Rx ring	*/
	uint32	rxTail;		/* Index of current tail of Rx ring	*/
	uint32	rxRingSize;	/* Size of Rx ring descriptor array	*/
	uint32	rxIrq;		/* Count of Rx interrupt requests       */

	void    *txRing; 	/* Ptr to array of xmit ring descriptors*/
	void    *txBufs; 	/* Ptr to Tx packet buffers in memory	*/
	uint32	txHead;		/* Index of current head of Tx ring	*/
	uint32	txTail;		/* Index of current tail of Tx ring	*/
	uint32	txRingSize;	/* Size of Tx ring descriptor array	*/
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

	int16 	proms; 		/* Nonzero => promiscuous mode 		*/

	int16 	ed_mcset;       /* Nonzero => multicast reception set   */
	int16 	ed_mcc;	 	/* Count of multicast addresses		*/
    	Eaddr   ed_mca[ETH_NUM_MCAST];/* Array of multicast addrs 	*/
};

extern	struct	ethcblk	ethertab[];	/* Array of control blocks      */
