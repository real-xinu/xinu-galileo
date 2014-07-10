/* e1000e_init.c - e1000e_init */

#include <xinu.h>

/*------------------------------------------------------------------------
 * e1000e_init - initialize Intel Hub 10D/82567LM Ethernet NIC
 *------------------------------------------------------------------------
 */
void 	e1000e_init(
	struct 	ether *ethptr
	)
{
	uint16  command;
	int32	i;
	uint32  rar_low, rar_high;

	/* Read PCI configuration information */

	/* Read I/O base address */

	pci_read_config_dword(ethptr->pcidev, E1000E_PCI_IOBASE,
			(uint32 *)&ethptr->iobase);
	ethptr->iobase &= ~1;
	ethptr->iobase &= 0xffff; /* the low bit is set to indicate I/O */

	/* Read flash base address */

	ethptr->flashbase = 0;
	pci_read_config_dword(ethptr->pcidev, E1000E_PCI_FLASHBASE,
			(uint32 *)&ethptr->flashbase);

	/* Read memory base address */

	pci_read_config_dword(ethptr->pcidev, E1000E_PCI_MEMBASE,
			(uint32 *)&ethptr->membase);
	ethptr->membase &= ~2; 	/* if mem address is below 1 MB */

	/* Read interrupt line number */

	pci_read_config_byte (ethptr->pcidev, E1000E_PCI_IRQ,
			(byte *)&(ethptr->dev->dvirq));

	/* Enable PCI bus master, memory access and I/O access */

	pci_read_config_word(ethptr->pcidev, E1000E_PCI_COMMAND, 
			&command);
	command |= E1000E_PCI_CMD_MASK;
	pci_write_config_word(ethptr->pcidev, E1000E_PCI_COMMAND, 
			command);

	/* Read the MAC address */
	
	rar_low = e1000e_io_readl(RAL(0));
	rar_high = e1000e_io_readl(RAH(0));

	for (i = 0; i < E1000E_RAL_MAC_ADDR_LEN; i++) 
		ethptr->devAddress[i] = (byte)(rar_low >> (i*8));
	for (i = 0; i < E1000E_RAH_MAC_ADDR_LEN; i++)
		ethptr->devAddress[i + 4] = (byte)(rar_high >> (i*8));

	kprintf("MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n",
			0xff&ethptr->devAddress[0],
			0xff&ethptr->devAddress[1],
			0xff&ethptr->devAddress[2],
			0xff&ethptr->devAddress[3],
			0xff&ethptr->devAddress[4],
			0xff&ethptr->devAddress[5]);
}
