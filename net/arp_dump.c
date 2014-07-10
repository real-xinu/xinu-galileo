/* arp_dump.c - arp_dump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * arp_dump - display the current ARP cache for an interface
 *------------------------------------------------------------------------
 */
void	arp_dump(
	  struct arpentry *arptr	/* pointer to entry in cache	*/
	)
{
	int32	i;			/* index into the ARP table	*/

	kprintf("State=%d, PID=%d  IP=%08x, HW=", arptr->arstate,
		arptr->arpid, arptr->arpaddr);
	kprintf(" %02X", arptr->arhaddr[0]);
	for (i = 1; i < ARP_HALEN; i++) {
			printf(":%02X", arptr->arhaddr[i]);
	}
	kprintf("\n");
	return;
}
