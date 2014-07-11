/* ethInit.c - ethInit */

#include <xinu.h>

struct	ether	ethertab[Neth];		/* Ethernet control blocks 	*/

/*------------------------------------------------------------------------
 * ethInit - Initialize Ethernet device structures
 *------------------------------------------------------------------------
 */
devcall	ethInit (
		struct dentry *devptr
		)
{
	struct	ether 	*ethptr;
	int32	dinfo;			/* device information		*/

	/* Initialize structure pointers */
	ethptr = &ethertab[devptr->dvminor];
	
	memset(ethptr, '\0', sizeof(struct ether));
	ethptr->dev = devptr;
	ethptr->csr = devptr->dvcsr;
	ethptr->state = ETH_STATE_DOWN;
	ethptr->mtu = ETH_MTU;
	ethptr->errors = 0;
	ethptr->addrLen = ETH_ADDR_LEN;
	ethptr->rxHead = ethptr->rxTail = 0;
	ethptr->txHead = ethptr->txTail = 0;

	if ((dinfo = find_pci_device(INTEL_QUARK_ETH_PCI_DID,
					INTEL_QUARK_ETH_PCI_VID, 0))
			!= SYSERR) {
		kprintf("Found Intel Quark Ethernet NIC\n");

		ethptr->type = ETH_TYPE_QUARK_ETH;
		ethptr->pcidev = dinfo;
		
		/* Initialize function pointers */
		
		ethptr->ethInit = quark_eth_init;
		ethptr->ethOpen = quark_eth_open;
		ethptr->ethRead = quark_eth_read;
		ethptr->ethWrite = quark_eth_write;
		ethptr->ethControl = quark_eth_cntl;
		ethptr->ethInterrupt = quark_eth_intr;
	}
	else {
		kprintf("No recognized PCI Ethernet NIC found\n");
		return SYSERR;
	}
	
	/* Initialize control block */
		
	ethptr->ethInit(ethptr);

	/* Allocate descriptors and buffers, active the NIC */

	if (ethptr->ethOpen(ethptr) != OK)
		return SYSERR;

	/* Ethernet interface is active from here */

	ethptr->state = ETH_STATE_UP;

	return OK;
}
