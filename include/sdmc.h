#define INTEL_QUARK_SDIO_PCI_DID	0x08A7	/* Intel Quark SDIO PCI Device ID	*/
#define INTEL_QUARK_SDIO_PCI_VID	0x8086	/* Intel Quark SDIO MAC PCI Vendor ID	*/

#ifndef	Nsdmc
#define	Nsdmc		1		/* number of SD Memory Cards		*/
#endif

struct sdmcblk {
	sid32	cmd_sem;		/* command semaphore 			*/
	sid32	tx_sem;			/* transfer semaphore			*/
};
extern	struct	sdmcblk	sdmctab[];

struct sdmc_csreg {
	uint32	sys_adr;		/* sdma system address register 	*/
	uint16	blk_size;		/* block size register 			*/
	uint16	blk_count;		/* block count register 		*/
	uint32	argument;		/* argument register 			*/
	uint16	tx_mode;		/* transfer mode register 		*/
	uint16	cmd;			/* command register 			*/
	uint32	response0;		/* response register 0 			*/
	uint32	response2;		/* response register 2 			*/
	uint32	response4;		/* response register 4 			*/
	uint32	response6;		/* response register 6 			*/
	uint32	buf_data_port;		/* buffer data port register 		*/
	uint32	pre_state;		/* present state register 		*/
	uint8	host_ctl;		/* host control register 		*/
	uint8	pwr_ctl;		/* power control register 		*/
	uint8	blk_gap_ctl;		/* block gap control register 		*/
	uint8	wakeup_ctl;		/* wakeup control register 		*/
	uint16	clk_ctl;		/* clock control register 		*/
	uint8	timeout_ctl;		/* timeout control register 		*/
	uint8	sw_rst;			/* software reset register 		*/
	uint16	nml_int_status;		/* normal interrupt status register 	*/
	uint16	err_int_status;		/* error interrupt status register 	*/
	uint16	nrm_int_status_en;	/* normal interrupt status enable 	*/
	uint16	err_int_stat_en;	/* error interrupt status enable register */
	uint16	nrm_int_sig_en;		/* normal interrupt signal enable register */
	uint16	err_int_sig_en;		/* error interrupt signal enable register */
	uint16	cmd12_err_stat;		/* auto cmd12 error status register 	*/
	uint16	host_ctrl_2;		/* host control 2 register 		*/
	uint32	capabilities;		/* capabilities register 		*/
	uint32	capabilities_2;		/* capabilities register 2 		*/
	uint32	max_cur_cap;		/* maximum current capabilities register */
	uint16	force_event_cmd12_err_stat;	/* force event register for auto cmd12 error status */
	uint16	force_event_err_int_stat;	/* force event register for error interrupt status */
	uint8	adma_err_stat;		/* adma error status register 		*/
	uint32	adma_sys_addr;		/* adma system address register */
	uint16	preset_value_0;		/* initialization preset values register */
	uint16	preset_value_1;		/* default speed preset values register */
	uint16	preset_value_2;		/* high speed preset values register */
	uint16	preset_value_3;		/* sdr12 preset values register */
	uint16	preset_value_4;		/* sdr25 preset values register */
	uint16	preset_value_5;		/* sdr50 preset values register */
	uint16	preset_value_6;		/* sdr104 preset values register */
	uint16	preset_value_7;		/* ddr50 preset values register */
	uint32	boot_timeout_ctrl;	/* boot time-out control register */
	uint8	debug_sel;		/* debug selection register */
	uint32	shared_bus;		/* shared bus control register */
	uint8	spi_int_sup;		/* spi interrupt support register */
	uint16	slot_int_stat;		/* slot interrupt status register */
	uint16	host_ctrl_ver;		/* host controller version register */
};

struct sdmc_cmd {
	uint16	resp_type_sel	:2;
	uint16	cmd_crc_chk_en	:1;
	uint16	cmd_index_chk_en:1;
	uint16	dat_pr_sel	:2;
	uint16	cmd_type	:2;
	uint16	cmd_index	:6;
	uint16	rsvd		:2;
};

struct sdmc_capabilities {
	uint32	slot_type		:2;	/* Slot Type				*/
	uint32	async_int_support	:1;	/* Asynchronous Interrupt Support	*/
	uint32	sys_bus_support_64b	:1;	/* 64-bit System Bus Support		*/
	uint32	rsvd4			:1;	/* RSVD4				*/
	uint32	volt_support_1p8v	:1;	/* Voltage Support 1.8V			*/
	uint32	volt_support_3p0v	:1;	/* Voltage Support 3.0V			*/
	uint32	volt_support_3p3v	:1;	/* Voltage Support 3.3V			*/
	uint32	suspend_resume_support	:1;	/* Suspend / Resume Support		*/
	uint32	sdma_support		:1;	/* SDMA Support				*/
	uint32	high_speed_support	:1;	/* High Speed Support			*/
	uint32	rsvd5			:1;	/* RSVD5				*/
	uint32	adma2_support		:1;	/* ADMA2 Support			*/
	uint32	extended_media_bus_support:1;	/* Extended Media Bus Support		*/
	uint32	max_block_length	:2;	/* Max Block Length			*/
	uint32	base_clock_frequency_sd_clk:8;	/* Base Clock Frequency for SD Clock	*/
	uint32	timeout_clock_unit	:1;	/* Timeout Clock Unit			*/
	uint32	rsvd6			:1;	/* RSVD6				*/
	uint32	timeout_clock_frequency	:6;	/* Timeout Clock Frequency		*/
};

/* Individual Bits in Control and Status Registers	*/

/* Normal Interrupt Status Enable */

#define SDMC_CMD_COMP_STAT_EN		0x0001	/* Command Complete Status Enable 	*/
#define SDMC_TX_COMP_STAT_EN		0x0002	/* Transfer Complete Status Enable	*/
#define SDMC_BLK_GAP_EVENT_STAT_EN	0x0004	/* Block Gap Event Status Enable	*/
#define SDMC_DMA_INT_STAT_EN		0x0008	/* DMA Interrupt Status Enable		*/
#define SDMC_BUF_WR_RDY_STAT_EN		0x0010	/* Buffer Write Ready Status Enable	*/
#define SDMC_BUF_RD_RDY_STAT_EN		0x0020	/* Buffer Read Ready Status Enable	*/
#define SDMC_CRD_INS_STAT_EN		0x0040	/* Card Insertion Status Enable		*/
#define SDMC_CRD_RMV_STAT_EN		0x0080	/* Card Removal Status Enable		*/
#define SDMC_CRD_INT_STAT_EN		0x0100	/* Card Interrupt Status Enable		*/
#define SDMC_INT_A_STAT_EN		0x0200	/* INT_A Status Enable			*/
#define SDMC_INT_B_STAT_EN		0x0400	/* INT_A Status Enable			*/
#define SDMC_INT_C_STAT_EN		0x0800	/* INT_A Status Enable			*/
#define SDMC_RE_TUNE_STAT_EN		0x1000	/* Re-Tuning Event Status Enable	*/

/* Normal Interrupt Signal Enable */

#define SDMC_CMD_COMP_SIG_EN		0x0001	/* Command Complete Status Enable 	*/
#define SDMC_TX_COMP_SIG_EN		0x0002	/* Transfer Complete Status Enable	*/
#define SDMC_BLK_GAP_EVENT_SIG_EN	0x0004	/* Block Gap Event Status Enable	*/
#define SDMC_DMA_INT_SIG_EN		0x0008	/* DMA Interrupt Status Enable		*/
#define SDMC_BUF_WR_RDY_SIG_EN		0x0010	/* Buffer Write Ready Status Enable	*/
#define SDMC_BUF_RD_RDY_SIG_EN		0x0020	/* Buffer Read Ready Status Enable	*/
#define SDMC_CRD_INS_SIG_EN		0x0040	/* Card Insertion Status Enable		*/
#define SDMC_CRD_RMV_SIG_EN		0x0080	/* Card Removal Status Enable		*/
#define SDMC_CRD_INT_SIG_EN		0x0100	/* Card Interrupt Status Enable		*/
#define SDMC_INT_A_SIG_EN		0x0200	/* INT_A Status Enable			*/
#define SDMC_INT_B_SIG_EN		0x0400	/* INT_A Status Enable			*/
#define SDMC_INT_C_SIG_EN		0x0800	/* INT_A Status Enable			*/
#define SDMC_RE_TUNE_SIG_EN		0x1000	/* Re-Tuning Event Status Enable	*/

/* Normal Interrupt Status Register */

#define SDMC_NML_INT_CMD_COMP		0x0001	/* Command Complete			*/
#define SDMC_NML_INT_TX_COMP		0x0002	/* Transfer Complete			*/
#define SDMC_NML_INT_BLK_GAP_EVENT	0x0004	/* Block Gap Event			*/
#define SDMC_NML_INT_DMA_INT		0x0008	/* DMA Interrupt			*/
#define SDMC_NML_INT_BUF_WR_RDY		0x0010	/* Buffer Write Ready			*/
#define SDMC_NML_INT_BUF_RD_RDY		0x0020	/* Buffer Read Ready			*/
#define SDMC_NML_INT_CRD_INS		0x0040	/* Card Insertion			*/
#define SDMC_NML_INT_CRD_RM		0x0080	/* Card Removal				*/
#define SDMC_NML_INT_CRD_INT		0x0100	/* Card Interrupt			*/
#define SDMC_NML_INT_INT_A		0x0200	/* INT_A				*/
#define SDMC_NML_INT_INT_B		0x0400	/* INT_B				*/
#define SDMC_NML_INT_INT_C		0x0800	/* INT_C				*/
#define SDMC_NML_INT_RE_TUNE		0x1000	/* Re-Tuning Event			*/
#define SDMC_NML_INT_BOOT_CK_RCV	0x2000	/* Boot Acknowledge Received		*/
#define SDMC_NML_INT_BOOT_TER_INT	0x4000	/* Boot Terminate Interrupt		*/
#define SDMC_NML_INT_ERR_INT		0x1000	/* Error Interrupt			*/

/* Present State Status Register */

#define SDMC_PRE_STATE_CMD_INHIBIT_CMD	0x00000001	/* Command Inhibit		*/
#define SDMC_PRE_STATE_CMD_INHIBIT_DAT	0x00000002	/* Command Inhibit DAT		*/
#define SDMC_PRE_STATE_CRD_INS		0x00010000	/* Card Inserted		*/

/* Clock Control Register */

#define SDMC_CLK_CTL_INT_CLK_EN		0x0001	/* Internal Clock Enable		*/
#define SDMC_CLK_CTL_INT_CLK_STABLE	0x0002	/* Internal Clock Stable		*/
#define SDMC_CLK_CTL_SD_CLK_EN		0x0004	/* SD Clock Enable			*/
#define SDMC_CLK_CTL_CLK_GEN_SEL	0x0020	/* Clock Generator Select		*/
#define SDMC_CLK_CTL_SD_FREQ_HIGH_MASK	0x00FF	/* Mask to set SD clock frequency to max*/


#define SDMC_CMD_DELAY			100000		/* Delay in micro secs	*/