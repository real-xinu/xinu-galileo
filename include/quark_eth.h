/* quark_eth.h */

/* Definitions for Intel Quark Ethernet */

#define INTEL_ETH_QUARK_PCI_DID	0x0937		/* MAC PCI Device ID	*/
#define INTEL_ETH_QUARK_PCI_VID	0x8086		/* MAC PCI Vendor ID	*/

struct eth_q_csreg {
	uint32	maccr;		/* MAC Configuration Register		*/
	uint32	macff;		/* MAC Frame Filter Register		*/
	uint32	hthr;		/* Hash Table High Register		*/
	uint32	htlr;		/* Hash Table Low Register		*/
	uint32	gmiiar;		/* GMII Address Register		*/
	uint32	gmiidr;		/* GMII Data Register			*/
	uint32	fcr;		/* Flow Control Register		*/
	uint32	vlantag;	/* VLAV Tag Register			*/
	uint32	version;	/* Version Register			*/
	uint32	debug;		/* Debug Register			*/
	uint32	res1[4];	/* Skipped Addresses			*/
	uint32	ir;		/* Interrupt Register			*/
	uint32	imr;		/* Interrupt Mask Register		*/
	uint32	macaddr0h;	/* MAC Address0 High Register		*/
	uint32	macaddr0l;	/* MAC Address0 Low Register		*/
	uint32  res2[46];
	uint32  mmccr;		/* MAC Management Counter Cntl Register	*/
	uint32  mmcrvcir;	/* MMC Receive Interrupt Register	*/
	uint32  mmctxir;	/* MMC Transmit Interrupt Register	*/
	uint32	res3[957];	/* Skipped Addresses			*/
	uint32	bmr;		/* Bus Mode Register			*/
	uint32	tpdr;		/* Transmit Poll Demand Register	*/
	uint32	rpdr;		/* Receive Poll Demand Register		*/
	uint32	rdla;		/* Receive Descriptor List Addr		*/
	uint32	tdla;		/* Transmit Descriptor List Addr	*/
	uint32	sr;		/* Status Register			*/
	uint32	omr;		/* Operation Mode Register		*/
	uint32	ier;		/* Interrupt Enable Register		*/
};

/* Individual Bits in Control and Status Registers */

/* MAC Configuration Register	*/

#define ETH_QUARK_MACCR_PE2K	0x08000000	/* Enable 2K Packets	*/
#define ETH_QUARK_MACCR_WD	0x00800000	/* Watchdog Disable	*/
#define ETH_QUARK_MACCR_JD	0x00400000	/* Jabber Disable	*/
#define ETH_QUARK_MACCR_JE	0x00100000	/* Jumbo Frame Enable	*/

/* Inter-frame gap values */
#define ETH_QUARK_MACCR_IFG96	0x00000000	/* 96 bit times		*/
#define ETH_QUARK_MACCR_IFG88	0x00020000	/* 88 bit times		*/
#define ETH_QUARK_MACCR_IFG80	0x00040000	/* 80 bit times		*/
#define ETH_QUARK_MACCR_IFG40	0x000E0000	/* 40 bit times		*/
#define ETH_QUARK_MACCR_IFG64	0x00080000	/* 64 bit times		*/

#define ETH_QUARK_MACCR_DCRS	0x00010000	/* Dis. C. Sense dur TX	*/
#define ETH_QUARK_MACCR_RMIISPD10  0x00000000	/* RMII Speed = 10 Mbps	*/
#define ETH_QUARK_MACCR_RMIISPD100 0x00004000	/* RMII Speed = 100 Mbps*/
#define ETH_QUARK_MACCR_DO	0x00002000	/* Disable Receive Own	*/
#define ETH_QUARK_MACCR_LM	0x00001000	/* Loopback Mode Enable	*/
#define ETH_QUARK_MACCR_DM	0x00000800	/* Duplex Mode Enable	*/
#define ETH_QUARK_MACCR_IPC	0x00000400	/* Checksum Offload	*/
#define ETH_QUARK_MACCR_DR	0x00000200	/* Disable Retry	*/
#define ETH_QUARK_MACCR_ACS	0x00000080	/* Auto Pad or CRC Strip*/
#define ETH_QUARK_MACCR_DC	0x00000010	/* Deferral Check	*/
#define ETH_QUARK_MACCR_TE	0x00000008	/* Transmitter Enable	*/
#define ETH_QUARK_MACCR_RE	0x00000004	/* Receiver Enable	*/
#define ETH_QUARK_MACCR_PRELEN7	0x00000000	/* Preamble = 7 bytes	*/
#define ETH_QUARK_MACCR_PRELEN5	0x00000001	/* Preamble = 5 bytes	*/
#define ETH_QUARK_MACCR_PRELEN3	0x00000002	/* Preamble = 3 bytes	*/

#define ETH_QUARK_MMC_CNTFREEZ 0x00000008   /* Freeze MMC counter values*/
#define ETH_QUARK_MMC_CNTRST   0x00000001   /* Reset all cntrs to zero	*/

/* GMII Address Register	*/
#define ETH_QUARK_GMIIAR_PAMASK	0x0000F800	/* Phys Layer Addr Mask	*/
#define ETH_QUARK_GMIIAR_GRMASK	0x000007C0	/* GMII Register Mask	*/
#define ETH_QUARK_GMIIAR_CR	0x00000004	/* Clk Range = 100-150	*/
						/*    MHz for Quark	*/
#define ETH_QUARK_GMIIAR_GW	0x00000002	/* GMII Write Enable	*/
#define ETH_QUARK_GMIIAR_GB	0x00000001	/* GMII Busy		*/

/* Bus Mode Register */
#define ETH_QUARK_BMR_SWR	0x00000001	/* Software Reset	*/

/* Status Register */
#define ETH_QUARK_SR_MMCI	0x08000000	/* MAC MMC interrupt	*/
#define ETH_QUARK_SR_TS_SUSP	0x00600000	/* TX DMA is suspended	*/
#define ETH_QUARK_SR_NIS	0x00010000	/* Normal Int summary	*/
#define ETH_QUARK_SR_AIS	0x00008000	/* Abnorm Intrupt summ.	*/
#define ETH_QUARK_SR_RI		0x00000040	/* Receive Interrupt	*/
#define ETH_QUARK_SR_TI		0x00000001	/* Transmit Interrupt	*/

/* Operation Mode Register */
#define ETH_QUARK_OMR_TSF	0x00200000	/* Tx store and forward	*/
#define ETH_QUARK_OMR_ST	0x00002000	/* Start/Stop TX	*/
#define ETH_QUARK_OMR_SR	0x00000002	/* Start/Stop RX	*/

/* Interrupt Enable Register */
#define ETH_QUARK_IER_NIE	0x00010000	/* Enable Norm Int Summ.*/
#define ETH_QUARK_IER_AIE	0x00008000	/* Enable Abnnom "   "	*/
#define ETH_QUARK_IER_RIE	0x00000040	/* Enable RX Interrupt	*/
#define ETH_QUARK_IER_TIE	0x00000001	/* Enable TX Interrupt	*/

/* Quark Ethernet Transmit Descriptor */

struct eth_q_tx_desc {
	uint32	ctrlstat;	/* Control and status	*/
	uint16	buf1size;	/* Size of buffer 1	*/
	uint16	buf2size;	/* Size of buffer 2	*/
	uint32	buffer1;	/* Address of buffer 1	*/
	uint32	buffer2;	/* Address of buffer 2	*/
};

#define ETH_QUARK_TDCS_OWN	0x80000000	/* Descrip. owned by DMA*/
#define ETH_QUARK_TDCS_IC	0x40000000	/* Int on Completion	*/
#define ETH_QUARK_TDCS_LS	0x20000000	/* Last Segment		*/
#define ETH_QUARK_TDCS_FS	0x10000000	/* First Segment	*/
#define ETH_QUARK_TDCS_TER	0x00200000	/* Transmit End of Ring	*/
#define ETH_QUARK_TDCS_ES	0x00008000	/* Error Summary	*/

/* Quark Ethernet Receive Descriptor */

struct eth_q_rx_desc {
	uint32	status;		/* Desc status word	*/
	uint16	buf1size;	/* Size of buffer 1	*/
	uint16	buf2size;	/* Size of buffer 2	*/
	uint32	buffer1;	/* Address of buffer 1	*/
	uint32	buffer2;	/* Address of buffer 2	*/
};
#define	rdctl1	buf1size	/* Buffer 1 size field has control bits too */
#define	rdctl2	buf2size	/* Buffer 2 size field has control bits too */

#define ETH_QUARK_RDST_OWN	0x80000000	/* Descrip. owned by DMA*/
#define ETH_QUARK_RDST_ES	0x00008000	/* Error Summary	*/
#define ETH_QUARK_RDST_FS	0x00000200	/* First Segment	*/
#define ETH_QUARK_RDST_LS	0x00000100	/* Last segment		*/
#define ETH_QUARK_RDST_FTETH	0x00000020	/* Frame Type = Ethernet*/

#define ETH_QUARK_RDCTL1_DIC	0x8000	/* Dis. Int on Complet.	*/
#define ETH_QUARK_RDCTL1_RER	0x8000	/* Recv End of Ring	*/

#define ETH_QUARK_RX_RING_SIZE	32
#define ETH_QUARK_TX_RING_SIZE	16

#define ETH_QUARK_INIT_DELAY	500000		/* Delay in micro secs	*/
#define ETH_QUARK_MAX_RETRIES	3		/* Max retries for init	*/
