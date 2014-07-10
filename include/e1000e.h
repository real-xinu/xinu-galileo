/* e1000e.h - information for Intel Hub 10D/82567LM NIC */

#define INTEL_VENDOR_ID		0x8086
#define INTEL_82567LM_DEVICE_ID	0x10DE

/* PCI Configuration Registers */

#define E1000E_PCI_COMMAND 	0x04
#define E1000E_PCI_STATUS 	0x06
#define E1000E_PCI_MEMBASE 	0x10
#define E1000E_PCI_FLASHBASE 	0x14
#define E1000E_PCI_IOBASE 	0x18
#define E1000E_PCI_IRQ 		0x3C

#define E1000E_IO_IOADDR	0x00
#define E1000E_IO_IODATA	0x04

#define E1000E_PCI_CMD_MASK	0x107

#define E1000E_RX_RING_SIZE 	32
#define E1000E_TX_RING_SIZE 	16
#define E1000E_RING_BOUNDARY 	16

/* Receive Descriptor */

struct e1000e_rx_desc {
	uint64  buffer_addr;		/* Address of the descriptor's 	*/
					/*  data buffer 		*/
	uint16  length;			/* Length of data DMAed into 	*/
					/*  data buffer 		*/
	uint16  csum;		   	/* Packet checksum 		*/
	uint8   status;		 	/* Descriptor status 		*/
	uint8   errors;		 	/* Descriptor Errors 		*/
	uint16  special;
};

/* Transmit Descriptor */

struct e1000e_tx_desc {
	uint64  buffer_addr;		/* Address of the descriptor's  */
					/*  data buffer 		*/
	union {
	 uint32 data;
	 struct {
	  uint16 	length; 	/* Data buffer length 		*/
	  uint8 	cso;		/* Checksum offset 		*/
	  uint8 	cmd;		/* Descriptor control 		*/
	 } flags;
	} lower;
	union {
	 uint32 data;
	 struct {
	  uint8 	status; 	/* Descriptor status 		*/
	  uint8 	css;		/* Checksum start 		*/
	  uint16 	special;
	 } fields;
	} upper;
};

#define E1000E_RDSIZE 		sizeof(struct 	e1000e_rx_desc)
#define E1000E_TDSIZE 		sizeof(struct 	e1000e_tx_desc)

static 	inline 	void 	_e1000e_io_writel(
	uint32 	iobase,
	uint32 	reg,
	uint32 	val
	)
{
	outl(iobase + E1000E_IO_IOADDR, reg);
	outl(iobase + E1000E_IO_IODATA, val);
}

static 	inline 	uint32 	_e1000e_io_readl(
	uint32 	iobase,
	uint32 	reg
	)
{
	outl(iobase + E1000E_IO_IOADDR, reg);
	return inl(iobase + E1000E_IO_IODATA);
}

#define e1000e_io_writel(reg, val) 					\
	_e1000e_io_writel(ethptr->iobase, E1000E_##reg, (val))
#define e1000e_io_readl(reg) 						\
	_e1000e_io_readl(ethptr->iobase, E1000E_##reg)
#define e1000e_io_flush()	 					\
	_e1000e_io_readl(ethptr->iobase, E1000E_STATUS);

#define ADDR_BIT_MASK   		0xFFFFFF	
#define E1000E_ICH_FWSM_RSPCIPHY 	0x00000040 
					/* Reset PHY on PCI Reset */
#define E1000E_ICH_LAN_INIT_TIMEOUT 	1500
#define E1000E_ICH_RAR_ENTRIES 		7
#define SW_FLAG_TIMEOUT 		1000 	
					/* SW Semaphore flag timeout in */
					/* 	milliseconds 		*/

/* Register Defination */

#define E1000E_CTRL 		0x00000 /* Device Control - RW 		*/
#define E1000E_STATUS 		0x00008 /* Device Status - RO 		*/
#define E1000E_CTRL_EXT 	0x00018 /* Extended Device Control - RW */
#define E1000E_MDIC 		0x00020 /* MDI Control - RW 		*/
#define E1000E_ICR 		0x000C0	/* Interrupt Cause Read - R/clr */
#define E1000E_ITR 		0x000C4	/* Interrupt Throttling Rate RW */
#define E1000E_ICS 		0x000C8 /* Interrupt Cause Set - WO 	*/
#define E1000E_IMS 		0x000D0 /* Interrupt Mask Set - RW 	*/
#define E1000E_IMC 		0x000D8 /* Interrupt Mask Clear - WO 	*/
#define E1000E_RCTL 		0x00100 /* Rx Control - RW 		*/
#define E1000E_TCTL 		0x00400 /* Tx Control - RW 		*/
#define E1000E_TIPG 		0x00410 /* Tx Inter-packet gap -RW 	*/
#define E1000E_EXTCNF_CTRL 	0x00F00 /* Extended Configuration 	*/
					/* 	Control 		*/
#define E1000E_PBA 		0x01000 /* Packet Buffer Allocation - RW*/
#define E1000E_RDTR 		0x02820 /* Rx Delay Timer - RW 		*/
#define E1000E_RADV 		0x0282C /* Rx Interrupt Absolute Delay 	*/
					/* 	Timer - RW 		*/
#define E1000E_RDBAL(_n) 	((_n) < 4 ? (0x02800 + ((_n) * 0x100)) : \
					(0x0C000 + ((_n) * 0x40)))
#define E1000E_RDBAH(_n) 	((_n) < 4 ? (0x02804 + ((_n) * 0x100)) : \
					(0x0C004 + ((_n) * 0x40)))
#define E1000E_RDLEN(_n) 	((_n) < 4 ? (0x02808 + ((_n) * 0x100)) : \
					(0x0C008 + ((_n) * 0x40)))
#define E1000E_RDH(_n) 		((_n) < 4 ? (0x02810 + ((_n) * 0x100)) : \
					(0x0C010 + ((_n) * 0x40)))
#define E1000E_RDT(_n) 		((_n) < 4 ? (0x02818 + ((_n) * 0x100)) : \
					(0x0C018 + ((_n) * 0x40)))
#define E1000E_RXDCTL(_n) 	((_n) < 4 ? (0x02828 + ((_n) * 0x100)) : \
					(0x0C028 + ((_n) * 0x40)))
#define E1000E_TDBAL(_n) 	((_n) < 4 ? (0x03800 + ((_n) * 0x100)) : \
					(0x0E000 + ((_n) * 0x40)))
#define E1000E_TDBAH(_n) 	((_n) < 4 ? (0x03804 + ((_n) * 0x100)) : \
					(0x0E004 + ((_n) * 0x40)))
#define E1000E_TDLEN(_n) 	((_n) < 4 ? (0x03808 + ((_n) * 0x100)) : \
					(0x0E008 + ((_n) * 0x40)))
#define E1000E_TDH(_n) 		((_n) < 4 ? (0x03810 + ((_n) * 0x100)) : \
					(0x0E010 + ((_n) * 0x40)))
#define E1000E_TDT(_n) 		((_n) < 4 ? (0x03818 + ((_n) * 0x100)) : \
					(0x0E018 + ((_n) * 0x40)))
#define E1000E_TXDCTL(_n) 	((_n) < 4 ? (0x03828 + ((_n) * 0x100)) : \
					(0x0E028 + ((_n) * 0x40)))
#define E1000E_TARC(_n)		(0x03840 + ((_n) * 0x100))
#define E1000E_KABGTXD 		0x03004 /* AFE Band Gap Transmit Ref 	*/
					/* 	Data */
#define E1000E_RAL(_i) 		(((_i) <= 15) ? (0x05400 + ((_i) * 8)) : \
					(0x054E0 + ((_i - 16) * 8)))
#define E1000E_RAH(_i) 		(((_i) <= 15) ? (0x05404 + ((_i) * 8)) : \
					(0x054E4 + ((_i - 16) * 8)))
#define E1000E_TIDV 		0x03820 /* Tx Interrupt Delay Value - RW*/
#define E1000E_TADV 		0x0382C /* Tx Interrupt Absolute Delay 	*/
					/* 	Val - RW 		*/
#define E1000E_RXCSUM 		0x05000 /* Rx Checksum Control - RW 	*/
#define E1000E_RFCTL 		0x05008 /* Receive Filter Control 	*/
#define E1000E_MTA 		0x05200 /* Multicast Table Array 	*/
					/* 	- RW Array 		*/
#define E1000E_KMRNCTRLSTA 	0x00034 /* MAC-PHY interface - RW 	*/
#define E1000E_GCR 		0x05B00 /* 	PCI-Ex Control		*/
#define E1000E_FWSM 		0x05B54 /* FW Semaphore 		*/

/* Extended Device Control */

#define E1000E_CTRL_EXT_RO_DIS 	0x00020000 	/*Relaxed Ordering disable*/

/* Receive Descriptor bit definitions */

#define E1000E_RXD_STAT_DD 	0x01 		/* Descriptor Done 	*/

/* Receive Control */

#define E1000E_RCTL_EN 		0x00000002 	/* enable 		*/
#define E1000E_RCTL_SBP 	0x00000004 	/* store bad packet 	*/
#define E1000E_RCTL_LPE 	0x00000020 	/* long packet enable 	*/
#define E1000E_RCTL_LBM_NO 	0x00000000 	/* no loopback mode 	*/
#define E1000E_RCTL_DTYP_MASK 	0x00000C00 	/* Descriptor type mask */
#define E1000E_RCTL_RDMTS_HALF 	0x00000000 	/*rx desc min thresh size*/
#define E1000E_RCTL_MO_SHIFT 	12 		/*multicast offset shift*/
#define E1000E_RCTL_BAM 	0x00008000 	/* broadcast enable 	*/

/* these buffer sizes are valid if E1000E_RCTL_BSEX is 0 */

#define E1000E_RCTL_SZ_2048 	0x00000000 	/* rx buffer size 2048 */

/* these buffer sizes are valid if E1000E_RCTL_BSEX is 1 */

#define E1000E_RCTL_SZ_4096 	0x00030000 	/* rx buffer size 4096 	*/
#define E1000E_RCTL_PMCF 	0x00800000 	/*pass MAC control frames*/
#define E1000E_RCTL_BSEX 	0x02000000 	/* Buffer size extension */
#define E1000E_RCTL_SECRC 	0x04000000 	/* Strip Ethernet CRC 	*/
#define E1000E_RCTL_FLXBUF_MASK 0x78000000 	/* Flexible buffer size */

/* Device Control */

#define E1000E_CTRL_GIO_MASTER_DISABLE 0x00000004
					/*Blocks new Master reqs 	*/
#define E1000E_CTRL_SLU 	0x00000040	
					/* Set link up (Force Link) 	*/
#define E1000E_CTRL_FRCSPD 	0x00000800 	/* Force Speed 		*/
#define E1000E_CTRL_FRCDPX 	0x00001000 	/* Force Duplex 	*/
#define E1000E_CTRL_RST 	0x04000000 	/* Global reset 	*/
#define E1000E_CTRL_RFCE 	0x08000000 	
					/* Receive Flow Control enable 	*/
#define E1000E_CTRL_TFCE 	0x10000000 	
					/* Transmit flow control enable */
#define E1000E_CTRL_PHY_RST 	0x80000000 	/* PHY Reset 		*/

/* Device Status */

#define E1000E_STATUS_LAN_INIT_DONE 0x00000200  
					/* Lan Init Completion by NVM 	*/
#define E1000E_STATUS_PHYRA 	0x00000400      
					/* PHY Reset Asserted 		*/
#define E1000E_STATUS_GIO_MASTER_ENABLE 0x00080000 
					/* Master request status 	*/

/* Transmit Descriptor bit definitions */

#define E1000E_TXD_CMD_EOP 	0x01000000 	/* End of Packet 	*/
#define E1000E_TXD_CMD_IFCS 	0x02000000 	
					/* Insert FCS (Ethernet CRC) 	*/
#define E1000E_TXD_CMD_IC 	0x04000000 	/* Insert Checksum 	*/
#define E1000E_TXD_CMD_RS 	0x08000000 	/* Report Status 	*/
#define E1000E_TXD_CMD_DEXT 	0x20000000 
					/* Descriptor extension 	*/
#define E1000E_TXD_CMD_IDE 	0x80000000 	/* Enable Tidv register */
#define E1000E_TXD_STAT_DD 	0x00000001 	/* Descriptor Done 	*/

/* Transmit Control */

#define E1000E_TCTL_EN 		0x00000002 	/* enable tx 		*/
#define E1000E_TCTL_PSP 	0x00000008 	/* pad short packets 	*/
#define E1000E_TCTL_COLD 	0x003ff000 	/* collision distance 	*/
#define E1000E_TCTL_CT 		0x00000ff0 	/* collision threshold 	*/
#define E1000E_TCTL_RTLC 	0x01000000 	
					/* Re-transmit on late collision*/

/* Receive Checksum Control */

#define E1000E_RXCSUM_TUOFL 	0x00000200   	
					/* TCP/UDP checksum offload 	*/
#define	E1000_RAH_AV		0x80000000

/* Header split receive */

#define E1000E_RFCTL_NFSW_DIS 	0x00000040
#define E1000E_RFCTL_NFSR_DIS 	0x00000080

/* Extended Configuration Control and Size */

#define E1000E_EXTCNF_CTRL_SWFLAG 0x00000020
#define E1000E_KABGTXD_BGSQLBIAS 0x00050000

/* Interrupt Cause Read */

#define E1000E_ICR_TXDW 	0x00000001 
					/* Transmit desc written back 	*/
#define E1000E_ICR_LSC  	0x00000004 	/* Link Status Change 	*/
#define E1000E_ICR_RXSEQ 	0x00000008 	/* rx sequence error 	*/
#define E1000E_ICR_RXO 		0x00000040 	/* rx overrun 		*/
#define E1000E_ICR_RXT0 	0x00000080 	/*rx timer intr (ring 0)*/

/*
 * This defines the bits that are set in the Interrupt Mask
 * Set/Read Register.  Each bit is documented below:
 * 	o RXT0   = Receiver Timer Interrupt (ring 0)
 * 	o TXDW   = Transmit Descriptor Written Back
 * 	o RXSEQ  = Receive Sequence Error
 * 	o LSC    = Link Status Change
 */
#define IMS_ENABLE_MASK ( 						\
				E1000E_IMS_TXDW 		|	\
				E1000E_IMS_LSC 			|	\
				E1000E_IMS_RXSEQ 		|	\
				E1000E_IMS_RXT0 		|	\
				E1000E_IMS_RXO)

/* Interrupt Mask Set */

#define E1000E_IMS_TXDW E1000E_ICR_TXDW 	/* Tx desc written back */
#define E1000E_IMS_LSC 	E1000E_ICR_LSC 		/* Link Status Change 	*/
#define E1000E_IMS_RXSEQ E1000E_ICR_RXSEQ 	/* rx sequence error 	*/
#define E1000E_IMS_RXO  E1000E_ICR_RXO 		/* rx overrun 		*/
#define E1000E_IMS_RXT0 E1000E_ICR_RXT0 	/* rx timer intr 	*/

/* Interrupt Cause Set */

#define E1000E_ICS_TXDW E1000E_ICR_TXDW 	/* Tx desc written back */
#define E1000E_ICS_LSC 	E1000E_ICR_LSC 		/* Link Status Change 	*/
#define E1000E_ICS_RXO 	E1000E_ICR_RXO 		/* rx overrun 		*/
#define E1000E_ICS_RXT0 E1000E_ICR_RXT0 	/* rx timer intr 	*/

/* Transmit Descriptor Control */

#define E1000E_TXDCTL_PTHRESH 	0x0000003F 	
					/* TXDCTL Prefetch Threshold 	*/
#define E1000E_TXDCTL_WTHRESH 	0x003F0000 	
					/* TXDCTL Writeback Threshold 	*/
#define E1000E_TXDCTL_GRAN    	0x01000000 	/* TXDCTL Granularity 	*/
#define E1000E_TXDCTL_FULL_TX_DESC_WB 0x01010000 /* GRAN=1, WTHRESH=1 	*/
#define E1000E_TXDCTL_MAX_TX_DESC_PREFETCH 0x0100001F 
					/* GRAN=1, PTHRESH=31 		*/

/* Receive Address */
/*
 * Number of high/low register pairs in the RAR. The RAR (Receive Address
 * Registers) holds the directed and multicast addresses that we monitor.
 * Technically, we have 16 spots.  However, we reserve one of these spots
 * (RAR[15]) for our directed address used by controllers with
 * manageability enabled, allowing us room for 15 multicast addresses.
 */
#define E1000E_RAL_MAC_ADDR_LEN 	4
#define E1000E_RAH_MAC_ADDR_LEN 	2

/* Collision related configuration parameters */

#define E1000E_HDX_COLLISION_DISTANCE 	511
#define E1000E_COLD_SHIFT 		12

/* Default values for the transmit IPG register */

#define DEFAULT_82543_TIPG_IPGT_FIBER 	9
#define DEFAULT_82543_TIPG_IPGT_COPPER 	8

#define DEFAULT_82543_TIPG_IPGR1 	8
#define E1000E_TIPG_IPGR1_SHIFT 	10

#define DEFAULT_82543_TIPG_IPGR2 	6
#define DEFAULT_80003ES2LAN_TIPG_IPGR2 	7
#define E1000E_TIPG_IPGR2_SHIFT 	20

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */

#define REQ_TX_DESCRIPTOR_MULTIPLE 	8
#define REQ_RX_DESCRIPTOR_MULTIPLE 	8


/* Number of 100 microseconds we wait for PCI Express master disable */

#define MASTER_DISABLE_TIMEOUT 		800

/* Number of milliseconds we wait for PHY configuration done after */
/* 	MAC reset */

#define PHY_CFG_TIMEOUT 		100


/* PCI Express Control */
#define E1000E_GCR_RXD_NO_SNOOP 	0x00000001
#define E1000E_GCR_RXDSCW_NO_SNOOP 	0x00000002
#define E1000E_GCR_RXDSCR_NO_SNOOP 	0x00000004
#define E1000E_GCR_TXD_NO_SNOOP 	0x00000008
#define E1000E_GCR_TXDSCW_NO_SNOOP 	0x00000010
#define E1000E_GCR_TXDSCR_NO_SNOOP 	0x00000020

#define PCIE_NO_SNOOP_ALL ( 						\
				E1000E_GCR_RXD_NO_SNOOP 	|	\
				E1000E_GCR_RXDSCW_NO_SNOOP 	| 	\
				E1000E_GCR_RXDSCR_NO_SNOOP 	| 	\
				E1000E_GCR_TXD_NO_SNOOP 	| 	\
				E1000E_GCR_TXDSCW_NO_SNOOP 	| 	\
				E1000E_GCR_TXDSCR_NO_SNOOP)

/* PHY Control Register */
#define MII_CR_RESTART_AUTO_NEG 0x0200  /* Restart auto negotiation 	*/
#define MII_CR_AUTO_NEG_EN 	0x1000  /* Auto Neg Enable 		*/
#define MII_CR_RESET 		0x8000  /* 0 = normal, 1 = PHY reset 	*/

/* PHY Status Register */
#define MII_SR_LINK_STATUS 	0x0004 	/* Link Status 1 = link 	*/
#define MII_SR_AUTONEG_COMPLETE 0x0020 	/* Auto Neg Complete 		*/

/* Autoneg Advertisement Register */

#define NWAY_AR_10T_HD_CAPS 	0x0020 	/* 10T   Half Duplex Capable 	*/
#define NWAY_AR_10T_FD_CAPS 	0x0040 	/* 10T   Full Duplex Capable 	*/
#define NWAY_AR_100TX_HD_CAPS	0x0080 	/* 100TX Half Duplex Capable 	*/
#define NWAY_AR_100TX_FD_CAPS	0x0100  /* 100TX Full Duplex Capable 	*/
#define NWAY_AR_PAUSE 		0x0400 	/* Pause operation desired 	*/
#define NWAY_AR_ASM_DIR 	0x0800 	/*Asymmetric Pause Direction bit*/

/* 1000BASE-T Control Register */

#define CR_1000T_HD_CAPS 	0x0100 	/* Advertise 1000T HD capability*/
#define CR_1000T_FD_CAPS 	0x0200 	/* Advertise 1000T FD capability*/

/* PHY Registers defined by IEEE */

#define PHY_CONTROL 		0x00 	/* Control Register 		*/
#define PHY_STATUS 		0x01 	/* Status Register 		*/
#define PHY_AUTONEG_ADV 	0x04 	/* Autoneg Advertisement 	*/
#define PHY_1000T_CTRL 		0x09 	/* 1000Base-T Control Reg 	*/
#define PHY_PAGE_SELECT 	0x15 	/* Page Address Reg 		*/

#define PHY_PAGE_SHIFT 		5
#define BM_PHY_PAGE_SELECT 	22 	/* Page Select for BM 		*/
#define PHY_CONTROL_LB 		0x4000 	/* PHY Loopback bit 		*/

#ifndef ETH_ALEN
#define ETH_ALEN 		6
#endif

#define PHY_REVISION_MASK 	0xFFFFFFF0
#define MAX_PHY_REG_ADDRESS 	0x1F 	/* 5 bit address bus (0-0x1F) 	*/
#define MAX_PHY_MULTI_PAGE_REG 	0xF

/* M88E1000 Specific Registers */

#define M88E1000E_PHY_SPEC_CTRL 0x10 	/* PHY Specific Control Register*/

/* M88E1000 PHY Specific Control Register */

#define M88E1000E_PSCR_POLARITY_REVERSAL 0x0002 
					/* 1=Polarity Reverse enabled 	*/
/* Auto crossover enabled all speeds */
#define M88E1000E_PSCR_AUTO_X_MODE 	0x0060

/* BME1000 PHY Specific Control Register */
#define BME1000E_PSCR_ENABLE_DOWNSHIFT 	0x0800 /* 1 = enable downshift */

/* MDI Control */

#define E1000E_MDIC_REG_SHIFT 	16
#define E1000E_MDIC_PHY_ADDR  	2
#define E1000E_MDIC_PHY_SHIFT 	21
#define E1000E_MDIC_OP_WRITE  	0x04000000
#define E1000E_MDIC_OP_READ 	0x08000000
#define E1000E_MDIC_READY 	0x10000000
#define E1000E_MDIC_ERROR 	0x40000000

/* SerDes Control */

#define E1000E_GEN_POLL_TIMEOUT 	640

#define E1000E_KMRNCTRLSTA_OFFSET 	0x001F0000
#define E1000E_KMRNCTRLSTA_OFFSET_SHIFT 16
#define E1000E_KMRNCTRLSTA_REN          0x00200000

#define E1000E_KMRNCTRLSTA_TIMEOUTS 	0x4 	/* Kumeran Timeouts 	*/
#define E1000E_KMRNCTRLSTA_INBAND_PARAM 0x9 	
					/* Kumeran InBand Parameters 	*/

#define E1000E_MTA_NUM_ENTRIES 	32

#define E1000E_PBA_DEFAULT 	0x000E000A

#define E1000E_TIDV_DEFAULT 	8       /* Transmit Interrupt Delay 	*/
#define E1000E_TADV_DEFAULT 	32 	/* Transmit Absolute Interrupt 	*/
					/* 	Delay 			*/
#define E1000E_RDTR_DEFAULT 	0       /* Receive Interrupt Delay 	*/
#define E1000E_RADV_DEFAULT 	8       /* Receive Absolute Interrupt 	*/
					/* 	Delay 			*/
#define E1000E_ITR_DEFAULT 	20000   /* Interrupt Throttling Rate 	*/

/* Collision related configuration parameters */

#define E1000E_COLLISION_THRESHOLD 	15
#define E1000E_CT_SHIFT 		4
#define E1000E_COLLISION_DISTANCE 	63
