/* 3c905C_cmd.c - _3c905C_cmd, _3c905C_win_read8, 3c905C_win_read16, 	*/
/* 			_3c905C_win_read32, _3c905C_win_write8, 	*/
/* 			_3c905C_win_write16, _3c905C_win_write32 	*/

#include <xinu.h>

status	_3c905C_cmd(
	struct 	ether 	*ethptr,
	uint16 	cmd,
	uint16 	arg
	)
{
	int i;

	outw(ethptr->iobase + _3C905C_REG_COMMAND, cmd | arg);

	for (i = 0; i < 2000; i++) {
		if (! (inw(ethptr->iobase + _3C905C_REG_STATUS) & 
		       _3C905C_STATUS_CMDINPROGRESS)) 
			return OK;
	}

	kprintf("3c905C: Command %04x with argument %04x failed\n",
			cmd, arg);

	return SYSERR;
}

uint8 	_3c905C_win_read8(
	struct 	ether 	*ethptr, 
	uint16 	window,
	uint16 	addr
	)
{									
	uint8 ret;

	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	ret = inb(ethptr->iobase + addr) & 0x000000ff;
	return ret;
}		

uint16 	_3c905C_win_read16(
	struct 	ether 	*ethptr, 
	uint16	window,
	uint16 	addr
	)
{
	uint16 ret;

	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	ret = inw(ethptr->iobase + addr) & 0x0000ffff;
	return ret;
}

uint32 	_3c905C_win_read32(
	struct 	ether 	*ethptr, 
	uint16 	window,
	uint16 	addr
	)
{
	uint32 ret;
	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	ret = inl(ethptr->iobase + addr);
	return ret;
}		

void 	_3c905C_win_write8(
	struct 	ether *ethptr,
	uint8 	value,
	uint16 	window,
	uint16 	addr
	)
{
	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	outb(ethptr->iobase + addr, value);
}

void 	_3c905C_win_write16(
	struct 	ether *ethptr,
	uint16 	value,
	uint16 	window,
	uint16 	addr
	)
{
	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	outw(ethptr->iobase + addr, value);
}

void _3c905C_win_write32(
	struct 	ether *ethptr,
	uint32 	value,
	uint16 	window,
	uint16 	addr
	)
{
	outw(ethptr->iobase + _3C905C_REG_COMMAND, 
			_3C905C_CMD_SELECTWINDOW + window);
	outl(ethptr->iobase + addr, value);
}
