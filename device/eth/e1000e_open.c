/* e1000e_open.c - e1000e_oepn, e1000e_irq_disable, e1000e_irq_enable */

#include <xinu.h>

/* allocate descriptor ring statically */

local 	status 	e1000e_reset(struct ether *ethptr);
local 	status 	e1000e_configure(struct ether *ethptr);
local 	status 	e1000e_acquire_swflag(struct ether *ethptr);
local 	void 	e1000e_release_swflag(struct ether *ethptr);
local 	status 	e1000e_check_reset_block(struct ether *ethptr);
local 	status 	e1000e_read_phy_reg_mdic(struct ether *ethptr, 
			uint32 offset, uint16 *data);
local 	status 	e1000e_write_phy_reg_mdic(struct ether *ethptr, 
			uint32 offset, uint16 data);
local 	status 	e1000e_read_phy_reg(struct ether *ethptr, 
			uint32 offset, uint16 *data);
local 	status 	e1000e_write_phy_reg(struct ether *ethptr, 
			uint32 offset, uint16 data);
local 	status 	e1000e_read_kmrn_reg(struct ether *ethptr, 
			uint32 offset, uint16 *data);
local 	status 	e1000e_write_kmrn_reg(struct ether *ethptr, 
			uint32 offset, uint16 data);
local 	status 	e1000e_init_hw(struct ether *ethptr);
local 	status 	e1000e_reset_hw(struct ether *ethptr);
local 	void 	e1000e_configure_rx(struct ether *ethptr);
local 	void 	e1000e_configure_tx(struct ether *ethptr);

/*------------------------------------------------------------------------
 * e1000e_open - allocate resources and prepare hardware for transmit and 
 *                receive
 *------------------------------------------------------------------------
 */
status e1000e_open(
	struct 	ether 	*ethptr
	)
{
	struct	e1000e_tx_desc* txRingptr;
	struct	e1000e_rx_desc*	rxRingptr;
	int32	i;
	uint32  bufptr;

	/* Initialize structure pointers */

	ethptr->rxRingSize = E1000E_RX_RING_SIZE;
	ethptr->txRingSize = E1000E_TX_RING_SIZE;
	ethptr->isem = semcreate(0);
	ethptr->osem = semcreate(ethptr->txRingSize);

	/* Rings must be aligned on a 16-byte boundary */
	
	ethptr->rxRing = (void *)getmem((ethptr->rxRingSize + 1)
			* E1000E_RDSIZE);
	ethptr->txRing = (void *)getmem((ethptr->txRingSize + 1)
			* E1000E_TDSIZE);
	ethptr->rxRing = (void *)(((uint32)ethptr->rxRing + 0xf) & ~0xf);
	ethptr->txRing = (void *)(((uint32)ethptr->txRing + 0xf) & ~0xf);
	
	/* Buffers are highly recommended to be allocated on cache-line */
	/* 	size (64-byte for E8400) 				*/
	
	ethptr->rxBufs = (void *)getmem((ethptr->rxRingSize + 1) 
			* ETH_BUF_SIZE);
	ethptr->txBufs = (void *)getmem((ethptr->txRingSize + 1) 
			* ETH_BUF_SIZE);
	ethptr->rxBufs = (void *)(((uint32)ethptr->rxBufs + 0x3f) 
			& ~0x3f);
	ethptr->txBufs = (void *)(((uint32)ethptr->txBufs + 0x3f) 
			& ~0x3f);

	if ( (SYSERR == (uint32)ethptr->rxBufs) || 
	     (SYSERR == (uint32)ethptr->txBufs) ) {
		return SYSERR;
	}

	/* Set buffer pointers and rings to zero */
	
	memset(ethptr->rxBufs, '\0', ethptr->rxRingSize * ETH_BUF_SIZE);
	memset(ethptr->txBufs, '\0', ethptr->txRingSize * ETH_BUF_SIZE);
	memset(ethptr->rxRing, '\0', E1000E_RDSIZE * ethptr->rxRingSize);
	memset(ethptr->txRing, '\0', E1000E_TDSIZE * ethptr->txRingSize);

	/* Insert the buffer into descriptor ring */
	
	rxRingptr = (struct e1000e_rx_desc *)ethptr->rxRing;
	bufptr = (uint32)ethptr->rxBufs;
	for (i = 0; i < ethptr->rxRingSize; i++) {
		rxRingptr->buffer_addr = (uint64)bufptr;
		rxRingptr++;
		bufptr += ETH_BUF_SIZE;
	}

	txRingptr = (struct e1000e_tx_desc *)ethptr->txRing;
	bufptr = (uint32)ethptr->txBufs;
	for (i = 0; i < ethptr->txRingSize; i++) {
		txRingptr->buffer_addr = (uint64)bufptr;
		txRingptr++;
		bufptr += ETH_BUF_SIZE;
	}

	/* Reset the NIC to bring it into a known state and initialize it */

	if (OK != e1000e_reset(ethptr)) {
		kprintf("e1000e_open: fail to reset E1000E device\n");
		return SYSERR;
	}

	/* Configure the NIC */

	if (OK != e1000e_configure(ethptr)) {
		kprintf("e1000e_open: fail to configure E1000E device\n");
		return SYSERR;
	}

	/* Enable interrupt */
	
	set_evec(ethptr->dev->dvirq + IRQBASE, (uint32)ethDispatch);
	e1000e_irq_enable(ethptr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_reset - bring the hardware into a known good state
 *------------------------------------------------------------------------
 */
local status e1000e_reset(
	struct ether *ethptr
	)
{
	/* reset Packet Buffer Allocation to default */

	e1000e_io_writel(PBA, E1000E_PBA_DEFAULT);

	/* Allow time for pending master requests to run */
	
	if (e1000e_reset_hw(ethptr) != OK)
		return SYSERR;

	if (e1000e_init_hw(ethptr) != OK)
		return SYSERR;

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_configure - configure the hardware for Rx and Tx
 *------------------------------------------------------------------------
 */
local status e1000e_configure(
	struct 	ether *ethptr
	)
{
	e1000e_configure_rx(ethptr);
	e1000e_configure_tx(ethptr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_acquire_swflag - Acquire software control flag
 *------------------------------------------------------------------------
 */
local status e1000e_acquire_swflag(
	struct 	ether *ethptr
	)
{
	uint32 extcnf_ctrl, timeout = PHY_CFG_TIMEOUT;

	while (timeout) {
		extcnf_ctrl = e1000e_io_readl(EXTCNF_CTRL);
		if (!(extcnf_ctrl & E1000E_EXTCNF_CTRL_SWFLAG))
			break;

		MDELAY(1);
		timeout--;
	}

	if (!timeout) {
		return SYSERR;
	}

	timeout = SW_FLAG_TIMEOUT;

	extcnf_ctrl |= E1000E_EXTCNF_CTRL_SWFLAG;
	e1000e_io_writel(EXTCNF_CTRL, extcnf_ctrl);

	while (timeout) {
		extcnf_ctrl = e1000e_io_readl(EXTCNF_CTRL);
		if (extcnf_ctrl & E1000E_EXTCNF_CTRL_SWFLAG)
			break;

		MDELAY(1)
			timeout--;
	}

	if (!timeout) {
		extcnf_ctrl &= ~E1000E_EXTCNF_CTRL_SWFLAG;
		e1000e_io_writel(EXTCNF_CTRL, extcnf_ctrl);
		return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_release_swflag - Release software control flag
 *------------------------------------------------------------------------
 */
local void e1000e_release_swflag(
	struct 	ether *ethptr
	)
{
	uint32 extcnf_ctrl;

	extcnf_ctrl = e1000e_io_readl(EXTCNF_CTRL);
	extcnf_ctrl &= ~E1000E_EXTCNF_CTRL_SWFLAG;
	e1000e_io_writel(EXTCNF_CTRL, extcnf_ctrl);

	return;
}

/*------------------------------------------------------------------------
 * e1000e_check_reset_block - Check if PHY reset is blocked
 *------------------------------------------------------------------------
 */
local status e1000e_check_reset_block(
	struct 	ether *ethptr
	)
{
	uint32 fwsm;

	fwsm = e1000e_io_readl(FWSM);

	return (fwsm & E1000E_ICH_FWSM_RSPCIPHY) ? OK
		: SYSERR;
}

/*------------------------------------------------------------------------
 * e1000e_read_phy_reg_mdic - Read MDI control register
 *------------------------------------------------------------------------
 */
local status e1000e_read_phy_reg_mdic(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	*data
	)
{
	uint32 i, mdic = 0;

	if (offset > MAX_PHY_REG_ADDRESS) {
		return SYSERR;
	}

	mdic = ((offset << E1000E_MDIC_REG_SHIFT) |
			(E1000E_MDIC_PHY_ADDR << E1000E_MDIC_PHY_SHIFT) |
			(E1000E_MDIC_OP_READ));

	e1000e_io_writel(MDIC, mdic);

	for (i = 0; i < (E1000E_GEN_POLL_TIMEOUT * 3); i++) {
		DELAY(50);
		mdic = e1000e_io_readl(MDIC);
		if (mdic & E1000E_MDIC_READY)
			break;
	}
	if (!(mdic & E1000E_MDIC_READY)) {
		return SYSERR;
	}
	if (mdic & E1000E_MDIC_ERROR) {
		return SYSERR;
	}
	*data = (uint16) mdic;

	return OK;
}

/*------------------------------------------------------------------------
 *  e1000e_write_phy_reg_mdic - Write MDI control register
 *------------------------------------------------------------------------
 */
local status e1000e_write_phy_reg_mdic(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	data
	)
{
	uint32 i, mdic = 0;

	if (offset > MAX_PHY_REG_ADDRESS) {
		return SYSERR;
	}

	mdic = ( ((uint32)data) |
		 (offset << E1000E_MDIC_REG_SHIFT) |
		 (E1000E_MDIC_PHY_ADDR << E1000E_MDIC_PHY_SHIFT) |
		 (E1000E_MDIC_OP_WRITE) );

	e1000e_io_writel(MDIC, mdic);

	for (i = 0; i < (E1000E_GEN_POLL_TIMEOUT * 3); i++) {
		DELAY(50);
		mdic = e1000e_io_readl(MDIC);
		if (mdic & E1000E_MDIC_READY)
			break;
	}
	if (!(mdic & E1000E_MDIC_READY)) {
		return SYSERR;
	}
	if (mdic & E1000E_MDIC_ERROR) {
		return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 *  e1000e_read_phy_reg - Read BM PHY register
 *------------------------------------------------------------------------
 */
local status e1000e_read_phy_reg(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	*data
	)
{
	uint32 page = offset >> PHY_PAGE_SHIFT;

	if (e1000e_acquire_swflag(ethptr) != OK)
		return SYSERR;

	if (offset > MAX_PHY_MULTI_PAGE_REG)
		if (e1000e_write_phy_reg_mdic(ethptr, 
					BM_PHY_PAGE_SELECT, page)
				!= OK)
			return SYSERR;

	if (e1000e_read_phy_reg_mdic(ethptr,
				MAX_PHY_REG_ADDRESS & offset, data)
			!= OK)
		return SYSERR;

	e1000e_release_swflag(ethptr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_write_phy_reg - Write BM PHY register
 *------------------------------------------------------------------------
 */
local status e1000e_write_phy_reg(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	data
	)
{
	uint32 page = offset >> PHY_PAGE_SHIFT;

	if (e1000e_acquire_swflag(ethptr) != OK)
		return SYSERR;

	if (offset > MAX_PHY_MULTI_PAGE_REG)
		if (e1000e_write_phy_reg_mdic(ethptr,
					PHY_PAGE_SELECT, page)
				!= OK)
			return SYSERR;

	if (e1000e_write_phy_reg_mdic(ethptr,
				MAX_PHY_REG_ADDRESS & offset, data)
			!= OK)
		return SYSERR;

	e1000e_release_swflag(ethptr);
	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_read_kmrn_reg - Read kumeran register
 *------------------------------------------------------------------------
 */
local status e1000e_read_kmrn_reg(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	*data
	)
{
	uint32 kmrnctrlsta;

	if (e1000e_acquire_swflag(ethptr) != OK)
		return SYSERR;

	kmrnctrlsta = ((offset << E1000E_KMRNCTRLSTA_OFFSET_SHIFT) &
			E1000E_KMRNCTRLSTA_OFFSET) | E1000E_KMRNCTRLSTA_REN;
	e1000e_io_writel(KMRNCTRLSTA, kmrnctrlsta);

	DELAY(2);

	kmrnctrlsta = e1000e_io_readl(KMRNCTRLSTA);
	*data = (uint16)kmrnctrlsta;

	e1000e_release_swflag(ethptr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_write_kmrn_reg - Write kumeran register
 *------------------------------------------------------------------------
 */
local status e1000e_write_kmrn_reg(
	struct 	ether *ethptr,
	uint32 	offset,
	uint16 	data
	)
{
	uint32 kmrnctrlsta;

	if (e1000e_acquire_swflag(ethptr) != OK)
		return SYSERR;

	kmrnctrlsta = ((offset << E1000E_KMRNCTRLSTA_OFFSET_SHIFT) &
			E1000E_KMRNCTRLSTA_OFFSET) | data;
	e1000e_io_writel(KMRNCTRLSTA, kmrnctrlsta);

	DELAY(2);

	e1000e_release_swflag(ethptr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_reset_hw - Reset the hardware 
 *------------------------------------------------------------------------
 */
local status e1000e_reset_hw(
	struct 	ether *ethptr
	)
{
	uint32 ctrl, kab;
	uint32 dev_status;
	uint32 data, loop = E1000E_ICH_LAN_INIT_TIMEOUT;
	int32 timeout = MASTER_DISABLE_TIMEOUT;

	/* Disables PCI_express master access */

	{
		ctrl = e1000e_io_readl(CTRL);
		ctrl |= E1000E_CTRL_GIO_MASTER_DISABLE;
		e1000e_io_writel(CTRL, ctrl);

		while (timeout) {
			if (!(e1000e_io_readl(STATUS) &
			      E1000E_STATUS_GIO_MASTER_ENABLE))
				break;
			DELAY(100);
			timeout--;
		}

		if (!timeout) {
			return SYSERR;
		}
	}

	/* Masking off all interrupts */

	e1000e_io_writel(IMC, 0xffffffff);

	/* Disable the Transmit and Receive units. */

	e1000e_io_writel(RCTL, 0);
	e1000e_io_writel(TCTL, 0);
	e1000e_io_flush();

	MDELAY(10);

	ctrl = e1000e_io_readl(CTRL);

	if (e1000e_check_reset_block(ethptr) == OK)
		ctrl |= E1000E_CTRL_PHY_RST;

	if (e1000e_acquire_swflag(ethptr) != OK)
		return SYSERR;

	/* Issuing a global reset */

	e1000e_io_writel(CTRL, (ctrl | E1000E_CTRL_RST));
	MDELAY(20);

	e1000e_release_swflag(ethptr);

	if (ctrl & E1000E_CTRL_PHY_RST) {
		MDELAY(10);

		do {
			data = e1000e_io_readl(STATUS);
		    	data &= E1000E_STATUS_LAN_INIT_DONE;
		    	DELAY(100);
		} while ((!data) && --loop);

		data = e1000e_io_readl(STATUS);
		data &= ~E1000E_STATUS_LAN_INIT_DONE;
		e1000e_io_writel(STATUS, data);

		dev_status = e1000e_io_readl(STATUS);
		if (dev_status & E1000E_STATUS_PHYRA)
			e1000e_io_writel(STATUS, 
					dev_status & ~E1000E_STATUS_PHYRA);

		MDELAY(10);
	}

	e1000e_io_writel(IMC, 0xffffffff);
	e1000e_io_readl(ICR);

	kab = e1000e_io_readl(KABGTXD);
	kab |= E1000E_KABGTXD_BGSQLBIAS;
	e1000e_io_writel(KABGTXD, kab);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_init_hw - Initialize the hardware
 *------------------------------------------------------------------------
 */
local status e1000e_init_hw(
	struct 	ether *ethptr
	)
{
	uint16 	i;
	uint32 	rar_low, rar_high;
	uint32 	ctrl, gcr, reg;
	uint16 	kmrn_data;
	uint16 	mii_autoneg_adv_reg, mii_1000t_ctrl_reg;
	uint16 	phy_data, phy_ctrl, phy_status;

	/* Initialize required hardware bits */

	{
		reg = e1000e_io_readl(CTRL_EXT);
		reg |= (1 << 22);
		e1000e_io_writel(CTRL_EXT, reg);
	    
		reg = e1000e_io_readl(TXDCTL(0));
		reg |= (1 << 22);
		e1000e_io_writel(TXDCTL(0), reg);
	
		reg = e1000e_io_readl(TXDCTL(1));
		reg |= (1 << 22);
		e1000e_io_writel(TXDCTL(1), reg);
	    
		reg = e1000e_io_readl(TARC(0));
		reg |= (1 << 23) | (1 << 24) | (1 << 26) | (1 << 27);
		e1000e_io_writel(TARC(0), reg);

		reg = e1000e_io_readl(TARC(1));
		reg &= ~(1 << 28);
		reg |= (1 << 24) | (1 << 26) | (1 << 30);
		e1000e_io_writel(TARC(1), reg);

		reg = e1000e_io_readl(RFCTL);
		reg |= (E1000E_RFCTL_NFSW_DIS | E1000E_RFCTL_NFSR_DIS);
		e1000e_io_writel(RFCTL, reg);
	}

	/* Setup the receive address */

	rar_low = rar_high = 0;
	for (i = 1; i < E1000E_ICH_RAR_ENTRIES; i++) {
		e1000e_io_writel(RAL(i), rar_low);
	    	e1000e_io_flush();
	    	e1000e_io_writel(RAH(i), rar_high);
	    	e1000e_io_flush();
	}

	/* Zero out the Multicast HASH table */

	for (i = 0; i < E1000E_MTA_NUM_ENTRIES; i++)
		e1000e_io_writel(MTA + (i << 2), 0);

	/* Setup link and flow control */

	{
		kprintf("Setup link...\n");
		if (e1000e_check_reset_block(ethptr) != OK)
			return SYSERR;

		ctrl = e1000e_io_readl(CTRL);
		ctrl |= E1000E_CTRL_SLU;
		ctrl &= ~(E1000E_CTRL_FRCSPD | E1000E_CTRL_FRCDPX);
		e1000e_io_writel(CTRL, ctrl);

		if (e1000e_write_kmrn_reg(ethptr, 
			E1000E_KMRNCTRLSTA_TIMEOUTS, 0xFFFF) != OK)
			return SYSERR;
	    
		if (e1000e_read_kmrn_reg(ethptr, 
			E1000E_KMRNCTRLSTA_INBAND_PARAM, &kmrn_data) != OK)
			return SYSERR;

		kmrn_data |= 0x3F;

		if (e1000e_write_kmrn_reg(ethptr, 
			E1000E_KMRNCTRLSTA_INBAND_PARAM, kmrn_data) != OK)
			return SYSERR;

	    	if (e1000e_read_phy_reg(ethptr, 
			M88E1000E_PHY_SPEC_CTRL, &phy_data) != OK)
			return SYSERR;

		phy_data |= M88E1000E_PSCR_AUTO_X_MODE;	
		phy_data &= ~M88E1000E_PSCR_POLARITY_REVERSAL;
		phy_data |= BME1000E_PSCR_ENABLE_DOWNSHIFT;

		if (e1000e_write_phy_reg(ethptr, 
			M88E1000E_PHY_SPEC_CTRL, phy_data) != OK)
			return SYSERR;

	    	if (e1000e_read_phy_reg(ethptr,
			PHY_CONTROL, &phy_ctrl) != OK)
			return SYSERR;
		

		phy_ctrl |= MII_CR_RESET;

		if (e1000e_write_phy_reg(ethptr, 
			PHY_CONTROL, phy_ctrl) != OK)
			return SYSERR;
		

		DELAY(1);

		if (e1000e_read_phy_reg(ethptr,
			PHY_AUTONEG_ADV, &mii_autoneg_adv_reg) != OK)
			return SYSERR;

	    	if (e1000e_read_phy_reg(ethptr, 
			PHY_1000T_CTRL, &mii_1000t_ctrl_reg) != OK)
			return SYSERR;

		mii_autoneg_adv_reg |= (NWAY_AR_100TX_FD_CAPS |
					NWAY_AR_100TX_HD_CAPS |
					NWAY_AR_10T_FD_CAPS   |
					NWAY_AR_10T_HD_CAPS);
	    
		mii_1000t_ctrl_reg &= ~CR_1000T_HD_CAPS;
		mii_1000t_ctrl_reg |= CR_1000T_FD_CAPS;

		mii_autoneg_adv_reg &= ~(NWAY_AR_ASM_DIR | NWAY_AR_PAUSE);

		if (e1000e_write_phy_reg(ethptr, 
			PHY_AUTONEG_ADV, mii_autoneg_adv_reg) != OK)
			return SYSERR;

		if (e1000e_write_phy_reg(ethptr, 
			PHY_1000T_CTRL, mii_1000t_ctrl_reg) != OK)
			return SYSERR;

		if (e1000e_read_phy_reg(ethptr, 
			PHY_CONTROL, &phy_ctrl) != OK)
			return SYSERR;

	    	phy_ctrl |= (MII_CR_AUTO_NEG_EN | 
			     MII_CR_RESTART_AUTO_NEG);

		if (e1000e_write_phy_reg(ethptr,
			PHY_CONTROL, phy_ctrl) != OK)
			return SYSERR;

		for (;;) {
			if (e1000e_read_phy_reg(ethptr, 
			    	PHY_STATUS, &phy_status) != OK)
			DELAY(10);
		    
			if (e1000e_read_phy_reg(ethptr,
			    	PHY_STATUS, &phy_status) != OK)
				return SYSERR;
		
		if ( (phy_status & MII_SR_LINK_STATUS) && 
		     (phy_status &MII_SR_AUTONEG_COMPLETE) )
			break;
		
			MDELAY(100);
		}

		ctrl = e1000e_io_readl(CTRL);
		ctrl &= (~(E1000E_CTRL_TFCE | E1000E_CTRL_RFCE));
		e1000e_io_writel(CTRL, ctrl);
	}

	/* Set PCI-express capabilities */

	gcr = e1000e_io_readl(GCR);
	gcr &= ~(PCIE_NO_SNOOP_ALL);
	gcr |= ~(PCIE_NO_SNOOP_ALL);
	e1000e_io_writel(GCR, gcr);

	return OK;
}

/*------------------------------------------------------------------------
 * e1000e_configure_rx - Configure Receive Unit after Reset
 *------------------------------------------------------------------------
 */
local void e1000e_configure_rx(
	struct 	ether *ethptr
	)
{
	uint32 rctl, rxcsum;

	/* Program MC offset vector base */

	rctl = e1000e_io_readl(RCTL);
	rctl &= ~(3 << E1000E_RCTL_MO_SHIFT);
	rctl |= E1000E_RCTL_EN | 
		E1000E_RCTL_BAM |
		E1000E_RCTL_LBM_NO |
		E1000E_RCTL_RDMTS_HALF;

	/* Do not Store bad packets, do not pass MAC control frame, 	*/
	/* 	disable long packet receive and CRC strip 		*/
	
	rctl &= ~(E1000E_RCTL_SBP |
		  E1000E_RCTL_LPE |
		  E1000E_RCTL_SECRC |
		  E1000E_RCTL_PMCF);
	
	/* Use Legacy description type */
	
	rctl &= ~E1000E_RCTL_DTYP_MASK;

	/* Setup buffer sizes */

	rctl &= ~(E1000E_RCTL_BSEX |
		  E1000E_RCTL_SZ_4096 |
		  E1000E_RCTL_FLXBUF_MASK);
	rctl |= E1000E_RCTL_SZ_2048;

	/* Set the Receive Delay Timer Register, let driver be notified */
	/* 	immediately each time a new packet has been stored in 	*/
	/* 	memory 							*/

	e1000e_io_writel(RDTR, E1000E_RDTR_DEFAULT);
	e1000e_io_writel(RADV, E1000E_RADV_DEFAULT);

	/* IRQ moderation */

	e1000e_io_writel(ITR, 1000000000 / (E1000E_ITR_DEFAULT * 256));

	/* Setup the HW Rx Head and Tail Descriptor Pointers, the Base 	*/
	/* 	and Length of the Rx Descriptor Ring 			*/

	e1000e_io_writel(RDBAL(0), (uint32)ethptr->rxRing);
	e1000e_io_writel(RDBAH(0), 0);
	e1000e_io_writel(RDLEN(0), E1000E_RDSIZE * ethptr->rxRingSize);
	e1000e_io_writel(RDH(0), 0);
	e1000e_io_writel(RDT(0), ethptr->rxRingSize - E1000E_RING_BOUNDARY);

	/* Disable Receive Checksum Offload for TCP and UDP */

	rxcsum = e1000e_io_readl(RXCSUM);
	rxcsum &= ~E1000E_RXCSUM_TUOFL;
	e1000e_io_writel(RXCSUM, rxcsum);

	e1000e_io_writel(RCTL, rctl);
}

/*------------------------------------------------------------------------
 * e1000_configure_tx - Configure Transmit Unit after Reset
 *------------------------------------------------------------------------
 */
local void e1000e_configure_tx(
	struct 	ether *ethptr
	)
{
	uint32 	tctl, tipg, txdctl;
	uint32 	ipgr1, ipgr2;

	/* Set the transmit descriptor write-back policy for both queues */

	txdctl = e1000e_io_readl(TXDCTL(0));
	txdctl &= ~E1000E_TXDCTL_WTHRESH;
	txdctl |= E1000E_TXDCTL_GRAN;
	e1000e_io_writel(TXDCTL(0), txdctl);
	txdctl = e1000e_io_readl(TXDCTL(1));
	txdctl &= ~E1000E_TXDCTL_WTHRESH;
	txdctl |= E1000E_TXDCTL_GRAN;
	e1000e_io_writel(TXDCTL(1), txdctl);

	/* Program the Transmit Control Register */
	
	tctl = e1000e_io_readl(TCTL);
	tctl &= ~E1000E_TCTL_CT;
	tctl |= E1000E_TCTL_RTLC |
		E1000E_TCTL_EN |
		E1000E_TCTL_PSP |
		(E1000E_COLLISION_THRESHOLD << E1000E_CT_SHIFT);
	tctl &= ~E1000E_TCTL_COLD;
	tctl |= E1000E_COLLISION_DISTANCE << E1000E_COLD_SHIFT;

	/* Set the default values for the Tx Inter Packet Gap timer */
	
	tipg = DEFAULT_82543_TIPG_IPGT_COPPER; 	/*  8  */
	ipgr1 = DEFAULT_82543_TIPG_IPGR1;	/*  8  */
	ipgr2 = DEFAULT_82543_TIPG_IPGR2;	/*  6  */

	tipg |= ipgr1 << E1000E_TIPG_IPGR1_SHIFT;
	tipg |= ipgr2 << E1000E_TIPG_IPGR2_SHIFT;
	e1000e_io_writel(TIPG, tipg);

	/* Set the Tx Interrupt Delay register */
	
	e1000e_io_writel(TIDV, E1000E_TIDV_DEFAULT);
	e1000e_io_writel(TADV, E1000E_TADV_DEFAULT);

	/* Setup the HW Tx Head and Tail descriptor pointers */
	
	e1000e_io_writel(TDBAL(0), (uint32)ethptr->txRing);
	e1000e_io_writel(TDBAH(0), 0);
	e1000e_io_writel(TDLEN(0), E1000E_TDSIZE * ethptr->txRingSize);
	e1000e_io_writel(TDH(0), 0);
	e1000e_io_writel(TDT(0), 0);

	e1000e_io_writel(TCTL, tctl);
}
