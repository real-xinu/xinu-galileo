#include <xinu.h>

int	quark_eth_phy_reset(volatile struct quark_eth_csreg *);
uint16	quark_eth_phy_read(volatile struct quark_eth_csreg *, uint32);
void	quark_eth_phy_write(volatile struct quark_eth_csreg *, uint32, uint16);

void quark_eth_init(struct ether *ethptr) {

	volatile struct quark_eth_csreg *csrptr; /* Pointer to Ethernet CSRs */
	uint32	retries; /* No. of retries for reset */
	byte	intr_pin;		/* Value of interrupt pin	*/

	/* Read the CSR Memory base address from PCI Config space */
	pci_read_config_dword(ethptr->pcidev, 0x10, (uint32 *)&ethptr->csr);
	csrptr = (struct quark_eth_csreg *)ethptr->csr;
	
	if(pci_read_config_byte(ethptr->pcidev, 0x3D, &intr_pin) == SYSERR) {
		kprintf("[SDMC] Unable to retrieve interrupt pin\n");
		return SYSERR;
	}
	kprintf("%02X\n", intr_pin);

	/* Enable CSR Memory Space, Enable Bus Master */
	pci_write_config_word(ethptr->pcidev, 0x4, 0x0006);

	/* Reset the Ethernet MAC */
	csrptr->bmr |= 0x00000001;

	/* Wait for the MAC Reset process to complete */
	retries = 0;
	while(csrptr->bmr & 0x00000001) {
		delay(QUARK_ETH_INIT_DELAY);
		if((++retries) > QUARK_ETH_MAX_RETRIES)
			return SYSERR;
	}

	/* Fixed burst mode */
	csrptr->bmr |= 0x00010000;

	csrptr->omr |= 0x00200000;

	/* Reset the Ethernet PHY */
	quark_eth_phy_reset(csrptr);

	/* Set the interrupt handler */
	set_evec(ethptr->dev->dvirq, (uint32)ethDispatch);

	/* Set the MAC Speed = 100Mbps, Full Duplex mode */
	csrptr->maccr |= (QUARK_ETH_MACCR_RMIISPD100 |
			  QUARK_ETH_MACCR_DM);
	csrptr->maccr |= (0x30000000);
	
	/* Reset the MMC Counters */
	csrptr->mmccr |= QUARK_ETH_MMC_CNTFREEZ | QUARK_ETH_MMC_CNTRST;
	
	/* Retrieve the MAC address from SPI flash */
	get_quark_pdat_entry_data_by_id(QUARK_MAC1_ID, (char*)(ethptr->devAddress), ETH_ADDR_LEN);
	
	kprintf("MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n",
		0xff&ethptr->devAddress[0],
		0xff&ethptr->devAddress[1],
		0xff&ethptr->devAddress[2],
		0xff&ethptr->devAddress[3],
		0xff&ethptr->devAddress[4],
		0xff&ethptr->devAddress[5]);

	csrptr->macaddr0l = (uint32)(*((uint32 *)ethptr->devAddress));
	csrptr->macaddr0h = ((uint32)(*((uint16 *)(ethptr->devAddress + 4))) | 0x80000000);
}

int quark_eth_phy_reset(volatile struct quark_eth_csreg *csrptr) {

	uint16 	value; 	/* Variable to read in PHY registers 	*/
	uint32	retries;/* No.  of retries for reset 		*/

	/* Read the PHY control register (register 0) */
	value = quark_eth_phy_read(csrptr, 0);

	/* Set bit 15 in control register to reset the PHY */
	quark_eth_phy_write(csrptr, 0, (value | 0x8000));

	/* Wait for PHY reset process to complete */
	retries = 0;
	while(quark_eth_phy_read(csrptr, 0) & 0x8000) {
		delay(QUARK_ETH_INIT_DELAY);
		if((++retries) > QUARK_ETH_MAX_RETRIES)
			return SYSERR;
	}

	/* Check if the PHY has auto-negotiation capability */
	value = quark_eth_phy_read(csrptr, 1); /* PHY Status register */
	if(value & 0x0008) { /* Auto-negotiation capability present */

		/* Wait for the auto-negotiation process to complete */
		while((quark_eth_phy_read(csrptr, 1) & 0x0020) == 0);

		/* Wait for the Link to be Up */
		while((quark_eth_phy_read(csrptr, 1) & 0x0004) == 0);
	}
	else { /* Auto-negotiation capability not present */
		/* TODO Set Link speed = 100Mbps */
	}

	kprintf("Ethernet Link is Up\n");

	return OK;
}

uint16 quark_eth_phy_read(volatile struct quark_eth_csreg *csrptr, uint32 regnum) {

	uint32	retries; /* No. of retries for read */

	/* Wait for the MII to be ready */
	while(csrptr->gmiiar & QUARK_ETH_GMIIAR_GB);

	/* Prepare the GMII address register for read transaction */
	csrptr->gmiiar = (1 << 11)		| /* Physical Layer Address = 1	*/
			 (regnum << 6)		| /* PHY Register Number	*/
			 (QUARK_ETH_GMIIAR_CR)	| /* GMII Clock Range 100-150MHz*/
			 (QUARK_ETH_GMIIAR_GB);	  /* Start the transaction	*/

	/* Wait for the transaction to complete */
	retries = 0;
	while(csrptr->gmiiar & QUARK_ETH_GMIIAR_GB) {
		delay(QUARK_ETH_INIT_DELAY);
		if((++retries) > QUARK_ETH_MAX_RETRIES)
			return 0;
	}

	/* Transaction is complete, read the PHY register value from GMII data register */
	return (uint16)csrptr->gmiidr;
}

void quark_eth_phy_write(volatile struct quark_eth_csreg *csrptr, uint32 regnum, uint16 value) {

	uint32	retries; /* No. of retries for write */

	/* Wait for the MII to be ready */
	while(csrptr->gmiiar & QUARK_ETH_GMIIAR_GB);

	/* Write the value to be written in PHY register in GMII data register	*/
	csrptr->gmiidr = (uint32)value;

	/* Prepare the GMII address register for write transaction */
	csrptr->gmiiar = (1 << 11)		| /* Physical Layer Address = 1	*/
			 (regnum << 6)		| /* PHY Register Number	*/
			 (QUARK_ETH_GMIIAR_CR)	| /* GMII Clock Range 100-150MHz*/
			 (QUARK_ETH_GMIIAR_GW)	| /* Write transaction		*/
			 (QUARK_ETH_GMIIAR_GB);	  /* Start the transaction	*/

	/* Wait till the transaction is complete */
	retries = 0;
	while(csrptr->gmiiar & QUARK_ETH_GMIIAR_GB) {
		delay(QUARK_ETH_INIT_DELAY);
		if((++retries) > QUARK_ETH_MAX_RETRIES)
			return;
	}
}
