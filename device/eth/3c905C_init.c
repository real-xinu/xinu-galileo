/* 3c905C_init.c - _3c905C_init */

#include <xinu.h>

/*------------------------------------------------------------------------
 * _3c905C_init - initialize 3C905C Ethernet NIC
 *------------------------------------------------------------------------
 */
void	_3c905C_init(
	struct 	ether *ethptr
	)
{
	uint16 	command;
	uint16	tmpaddr;		/* two bytes of MAC address	*/

	/* Read PCI configuration information */
	/* Read I/O base address */

	pci_read_config_dword(ethptr->pcidev, _3C905C_PCI_IOBASE,
		(uint32 *)&ethptr->iobase);
	ethptr->iobase &= ~1;
	ethptr->iobase &= 0xffff; /* the low bit is set to indicate I/O */

	/* Read memory base address */

	pci_read_config_dword(ethptr->pcidev,_3C905C_PCI_MEMBASE,
		(uint32 *)&ethptr->membase);
	ethptr->membase &= ~2; 	/* If mem address is below 1 MB */
	
	/* Read interrupt line number */
	
	pci_read_config_byte(ethptr->pcidev,_3C905C_PCI_IRQ,
		(byte *)&(ethptr->dev->dvirq));

	/* Enable PCI bus master */

	pci_read_config_word(ethptr->pcidev, _3C905C_PCI_COMMAND,
			&command);
	command |= _3C905C_PCI_CMD_MASK;
	pci_write_config_word(ethptr->pcidev, _3C905C_PCI_COMMAND,
			command);

	/* Read the MAC address */ //TODO: should read from EEPROM

	tmpaddr = _3c905C_win_read16(ethptr, 2, 0x00);
	ethptr->devAddress[0] = tmpaddr & 0xff;
	ethptr->devAddress[1] = 0xff & (tmpaddr >> 8);
	tmpaddr = _3c905C_win_read16(ethptr, 2, 0x02);
	ethptr->devAddress[2] = tmpaddr & 0xff;
	ethptr->devAddress[3] = 0xff & (tmpaddr >> 8);
	tmpaddr = _3c905C_win_read16(ethptr, 2, 0x04);
	ethptr->devAddress[4] = tmpaddr & 0xff;
	ethptr->devAddress[5] = 0xff & (tmpaddr >> 8);

	kprintf("MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n",
			0xff&ethptr->devAddress[0],
			0xff&ethptr->devAddress[1],
			0xff&ethptr->devAddress[2],
			0xff&ethptr->devAddress[3],
			0xff&ethptr->devAddress[4],
			0xff&ethptr->devAddress[5]);
}
