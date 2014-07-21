#define INTEL_QUARK_SDIO_PCI_DID	0x08A7	/* Intel Quark SDIO PCI Device ID	*/
#define INTEL_QUARK_SDIO_PCI_VID	0x8086	/* Intel Quark SDIO MAC PCI Vendor ID	*/

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