/* sdmcOpen.c  -  sdmcOpen */

#include <xinu.h>
#include <sdmc.h>

devcall sdmc_issue_cmd (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 uint16 cmd_value,		/* SDMC Command to issue	*/
	 uint32 arg_value		/* Command argument		*/
	)
{
	struct	sdmc_csreg *csrptr;	/* address of SD controller's CSR	*/
	struct	sdmcblk	*sdmcptr;	/* Pointer to sdmctab entry	*/
		
	/* Initialize structure pointers */
	
	sdmcptr = &sdmctab[devptr->dvminor];
	csrptr = (struct sdmc_csreg *) devptr->dvcsr;
	
	/* Wait for command inhibit bits to be cleared */
	
	kprintf("Before delay: %08X %04X %08X\n", csrptr->pre_state, cmd_value, arg_value);
	
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_CMD) {
		DELAY(SDMC_CMD_DELAY);
	}
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_DAT) {
		DELAY(SDMC_CMD_DELAY);
	}
	kprintf("After delay: %08X\n", csrptr->pre_state);
	
	/* Issue the command */
	csrptr->argument = arg_value;
	csrptr->cmd = cmd_value;
	
	kprintf("Command issued: %08X\n", csrptr->pre_state);
	
	return OK;
}

devcall sdmc_set_clock (
	 struct sdmc_csreg	*csrptr		/* address of SD controller's CSR	*/
	)
{
	/* Change to the SD Clock is not allowed while command inhibit is set */
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_CMD) {
		DELAY(SDMC_CMD_DELAY);
	}
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_DAT) {
		DELAY(SDMC_CMD_DELAY);
	}
	
	/* Disable the SD Clock */
	csrptr->clk_ctl &= ~(SDMC_CLK_CTL_SD_CLK_EN);
	
	/* Set the SD clock frequency divisor */
	csrptr->clk_ctl &= SDMC_CLK_CTL_SD_FREQ_HIGH_MASK;
	
	/* Internal enable the SD clock */
	csrptr->clk_ctl &= SDMC_CLK_CTL_INT_CLK_EN;
	
	/* Wait for SD clock to be stable */
	while(!(csrptr->clk_ctl & SDMC_CLK_CTL_INT_CLK_STABLE)) {
		DELAY(SDMC_CMD_DELAY);
	}
	
	/* Set SD Clock enable */
	csrptr->clk_ctl &= SDMC_CLK_CTL_SD_CLK_EN;
	
	kprintf("%08X\n", csrptr->capabilities);
	
	if(((struct sdmc_capabilities*)&csrptr->capabilities)->base_clock_frequency_sd_clk != 0) {
		kprintf("%d\n", ((struct sdmc_capabilities*)&csrptr->capabilities)->base_clock_frequency_sd_clk);
	}
	
	return OK;
}	
	

/*------------------------------------------------------------------------
 * sdmcOpen - open an SD memory card
 *------------------------------------------------------------------------
 */
devcall	sdmcOpen (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 char	*name,			/* name to open			*/
	 char	*mode			/* mode argument		*/
	)
{
	struct	sdmc_csreg *csrptr;	/* address of SD controller's CSR	*/
	struct	sdmcblk	*sdmcptr;	/* Pointer to sdmctab entry	*/
	struct	sdmc_cmd cmd;		/* SDMC command to issue	*/
	
	/* Initialize structure pointers */
	
	sdmcptr = &sdmctab[devptr->dvminor];
	csrptr = (struct sdmc_csreg *) devptr->dvcsr;
	
	sdmcptr->cmd_sem = semcreate(0);
	if((int)sdmcptr->cmd_sem == SYSERR) {
		return SYSERR;
	}
	
	/* Enable and register for command interrupts */
	
	//csrptr->nrm_int_status_en |= SDMC_CMD_COMP_STAT_EN | SDMC_TX_COMP_STAT_EN | SDMC_CRD_INT_STAT_EN;
	//csrptr->nrm_int_sig_en |= SDMC_CMD_COMP_SIG_EN | SDMC_TX_COMP_SIG_EN | SDMC_CRD_INT_SIG_EN;
	
	kprintf("%04X %04X\n", csrptr->nrm_int_status_en, csrptr->nrm_int_sig_en);
	
	sdmc_set_clock(csrptr);
	
	/* Issue card reset command (CMD0) */
	//(*(uint16*)&cmd) = 0;
	//cmd.cmd_index = 0;
	//cmd.resp_type_sel = 2;
	//sdmc_issue_cmd(devptr, (*(uint16*)&cmd), 0);
	//wait(sdmcptr->cmd_sem);
	
	//kprintf("command completed: %08X\n", csrptr->pre_state);
	
	return devptr->dvnum;
}
