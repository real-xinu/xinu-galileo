/* sdmcOpen.c  -  sdmcOpen */

#include <xinu.h>
#include <sdmc.h>

devcall sdmc_cmd_err_rcvy (
	 volatile struct sdmc_csreg *csrptr,	/* address of SD controller's CSR	*/
	 uint16* error_sts			/* command error status			*/
	)
{
	kprintf("[SDMC] Error recovery %04X\n", csrptr->err_int_status);
	
	if(csrptr->err_int_status &
		(SDMC_ERR_INT_CMD_TIMEOUT_ERR | 
		SDMC_ERR_INT_CMD_CRC_ERR |
		SDMC_ERR_INT_CMD_END_BIT_ERR |
		SDMC_ERR_INT_CMD_INDEX_ERR)) {
		
		csrptr->sw_rst |= SDMC_SW_RST_CMD_LN;
		while(csrptr->sw_rst & SDMC_SW_RST_CMD_LN) {
			DELAY(SDMC_CMD_DELAY);
		}
	}
	
	if(csrptr->err_int_status & 
		(SDMC_ERR_INT_DATA_TIMEOUT_ERR |
		SDMC_ERR_INT_DATA_CRC_ERR |
		SDMC_ERR_INT_DATA_END_BIT_ERR)) {

		csrptr->sw_rst |= SDMC_SW_RST_DAT_LN;
		while(csrptr->sw_rst & SDMC_SW_RST_DAT_LN) {
			DELAY(SDMC_CMD_DELAY);
		}
	}
	
	kprintf("After software reset\n");
	
	/* Savea and clear the error status */
	(*error_sts) = csrptr->err_int_status;
	csrptr->err_int_status |= (*error_sts);
	
	/* Issue the abort command (CMD12) */
	csrptr->argument = 0;
	csrptr->cmd = 0x0C00;
	
	/* Wait for command and DAT inhibit signals to be cleared */
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_CMD) {
		DELAY(SDMC_CMD_DELAY);
	}
	while(csrptr->pre_state & SDMC_PRE_STATE_CMD_INHIBIT_DAT) {
		DELAY(SDMC_CMD_DELAY);
	}
	
	/* Check status of abort command */
	if(csrptr->err_int_status &
		(SDMC_ERR_INT_CMD_TIMEOUT_ERR | 
		SDMC_ERR_INT_CMD_CRC_ERR |
		SDMC_ERR_INT_CMD_END_BIT_ERR |
		SDMC_ERR_INT_CMD_INDEX_ERR)) {
		return SDMC_RC_NON_RECOVERABLE_ERROR;
	}
	
	/* Check for data timeout */
	if(csrptr->err_int_status & SDMC_ERR_INT_DATA_TIMEOUT_ERR) {
		return SDMC_RC_NON_RECOVERABLE_ERROR;
	}
	
	/* Check data line signal level */
	DELAY(1); 	/* Must wait at least 40 microseconds */
	if(csrptr->pre_state & SDMC_PRE_STATE_DATA_LN_SIG_LVL) {
		return SDMC_RC_RECOVERABLE_ERR;
	}
	return SDMC_RC_NON_RECOVERABLE_ERROR;
}

devcall sdmc_issue_cmd (
	 volatile struct sdmc_csreg *csrptr,	/* address of SD controller's CSR	*/
	 uint16 cmd_value,			/* SDMC Command to issue	*/
	 uint32 arg_value,			/* Command argument		*/
	 uint16* error_sts			/* Error status			*/
	)
{
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
	
	while(!(csrptr->nml_int_status & SDMC_NML_INT_CMD_COMP) && 
	      !(csrptr->nml_int_status & SDMC_NML_INT_ERR_INT)) {
		kprintf("%08X %04X %04X\n", csrptr->pre_state, csrptr->nml_int_status, csrptr->err_int_status);
		DELAY(SDMC_CMD_DELAY);
	}
	
	kprintf("CMD: %04X %04X\n", csrptr->nml_int_status, csrptr->err_int_status);
	
	kprintf("Command done: %08X %08X\n", csrptr->pre_state, csrptr->response0);
	
	/* Clear the command complete interrupt */
	if(csrptr->nml_int_status & SDMC_NML_INT_CMD_COMP) {
		csrptr->nml_int_status |= SDMC_NML_INT_CMD_COMP;
	}
	
	/* Clear the error interrupt and perform error recovery */
	if(csrptr->nml_int_status & SDMC_NML_INT_ERR_INT) {
	
		csrptr->nml_int_status |= SDMC_NML_INT_ERR_INT;
		
		if(sdmc_cmd_err_rcvy(csrptr, error_sts) == SDMC_RC_NON_RECOVERABLE_ERROR) {
			return SYSERR;
		}
	}
	
	return OK;
}

devcall sdmc_set_clock (
	 volatile struct sdmc_csreg *csrptr	/* address of SD controller's CSR	*/
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
	csrptr->clk_ctl |= SDMC_CLK_CTL_SD_CLK_EN;
	
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
	volatile struct	sdmc_csreg *csrptr;	/* address of SD controller's CSR	*/
	struct	sdmcblk	*sdmcptr;	/* Pointer to sdmctab entry	*/
	uint16	error_sts = 0;		/* SDMC command error status	*/
	uint8	flag8 = 1;		/* F8 as described by the SD specification	*/
	
	/* Initialize structure pointers */
	
	sdmcptr = &sdmctab[devptr->dvminor];
	csrptr = (struct sdmc_csreg *) devptr->dvcsr;
	
	/* Set the clock speed */
	sdmc_set_clock(csrptr);
	
	/* Enable and register for command interrupts */
	csrptr->nrm_int_status_en |= SDMC_CMD_COMP_STAT_EN | SDMC_TX_COMP_STAT_EN | SDMC_CRD_INT_STAT_EN;
	//csrptr->nrm_int_sig_en |= SDMC_CMD_COMP_SIG_EN | SDMC_TX_COMP_SIG_EN | SDMC_CRD_INT_SIG_EN;
	csrptr->err_int_stat_en |= SDMC_ERR_INT_CMD_TIMEOUT_ERR_STAT_EN | SDMC_ERR_INT_CMD_CRC_ERR_STAT_EN | SDMC_ERR_INT_CMD_END_BIT_ERR_STAT_EN | SDMC_ERR_INT_CMD_IND_ERR_STAT_EN;
	//csrptr->err_int_sig_en |=  SDMC_ERR_INT_CMD_TIMEOUT_ERR_SIG_EN | SDMC_ERR_INT_CMD_CRC_ERR_SIG_EN | SDMC_ERR_INT_CMD_END_BIT_ERR_SIG_EN | SDMC_ERR_INT_CMD_IND_ERR_SIG_EN;
	
	kprintf("%04X %04X %08X\n", csrptr->nrm_int_status_en, csrptr->nrm_int_sig_en, csrptr->pre_state);
	
	/* Issue card reset command (CMD0) */
	if(sdmc_issue_cmd(csrptr, SDMC_CMD_CMD0, 0, &error_sts) == SYSERR) {
		kprintf("[SDMC] Error in CMD0: %04X %04X", SDMC_CMD_CMD0, error_sts);
		return SYSERR;
	}
	
	/* Issue voltage check command (CMD8) */
	if(sdmc_issue_cmd(csrptr, SDMC_CMD_CMD8, SDMC_ARG_CMD8, &error_sts) == SYSERR) {
		kprintf("[SDMC] Error in CMD8: %04X %04X", SDMC_CMD_CMD8, error_sts);
		return SYSERR;
	}
	if(error_sts & SDMC_ERR_INT_CMD_TIMEOUT_ERR) {
		flag8 = 0;
	}
	
	if(sdmc_issue_cmd(csrptr, SDMC_CMD_ACMD41, SDMC_ARG_ACMD41_INQUIRY, &error_sts) == SYSERR) {
		kprintf("[SDMC] Error in ACMD41: %04X %04X", SDMC_CMD_ACMD41, error_sts);
		return SYSERR;
	}
	
	kprintf("command completed: %08X %08X\n", csrptr->pre_state, csrptr->response0);
	
	sdmcptr->cmd_sem = semcreate(0);
	if((int)sdmcptr->cmd_sem == SYSERR) {
		return SYSERR;
	}
	
	return devptr->dvnum;
}
