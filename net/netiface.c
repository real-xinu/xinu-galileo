/* netiface.c - netiface_init */

#include <xinu.h>

/* Functions used to initialize interfaces and create an input process	*/
/*	for each interface						*/

bpid32	netbufpool;			/* ID of network buffer pool	*/

struct	ifentry	if_tab[NIFACES];	/* Array of network interfaces	*/

int32	ifprime	= 0;			/* primary interface		*/

/*------------------------------------------------------------------------
 * netiface_init - initialize the network interface data structures
 *------------------------------------------------------------------------
 */
void	netiface_init (void) {

	int32	iface;			/* index into interface table	*/
	struct	ifentry	*ifptr;		/* ptr to interface table entry	*/

	/* Initialize interfaces for Ehternets and Othernets */

	for (iface=0; iface<NIFACES; iface++) {
		ifptr = &if_tab[iface];

		/* initialize entire structure to zero */

		memset((char *)ifptr, NULLCH, sizeof(struct ifentry));

		/* Record the device ID of the underlying device (NIC)	*/

		ifptr->if_dev = ethertab[0].dev -> dvnum;

		if (iface == 0) {

			/* Set Ethernet MAC address in interface 0 */

			control(ifptr->if_dev, ETH_CTRL_GET_MAC,
					(int32) ifptr->if_macucast, 0);
			memset(ifptr->if_macbcast, 0xff, ETH_ADDR_LEN);
		}

		ifptr->if_state = IF_DOWN;	/* interface is down	*/
		ifptr->if_ipvalid = FALSE;	/* IP fields are invalid*/

		/* create the buffer synchronization semaphore */

		ifptr->if_sem = semcreate(0);	/* no packets in buffer	*/
		if (ifptr->if_sem == SYSERR) {
			panic("netiface_init: cannot create semaphore");
		}
	}

	/* Enable Ethernet interface but no others */

	if_tab[0].if_state = IF_UP;
	return;
}
