/* netstart.c - netstart */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * netstart - initialize network and run DHCP to get IP address
 *------------------------------------------------------------------------
 */

void	netstart (void)
{
	uint32	ipaddr;			/* IP address on interface 0	*/

	/* Initialize network interfaces */

	kprintf("...initializing network stack\n");
	net_init();

	/* Delay because Ethernet driver doesn't work without it */

	//sleepms(800);
	
	/* Force system to use DHCP to obtain an address */

	kprintf("...using dhcp to obtain an IP address\n");
	ipaddr = getlocalip();
	if (ipaddr == SYSERR) {
		panic("Error: could not obtain an IP address\n\r");
	}
	kprintf("\nIP address is %d.%d.%d.%d   (0x%08x)\n\r",
		(ipaddr>>24)&0xff, (ipaddr>>16)&0xff, (ipaddr>>8)&0xff,
		ipaddr&0xff,ipaddr);

	kprintf("Subnet mask is %d.%d.%d.%d and router is %d.%d.%d.%d\n\r",
		(NetData.ipmask>>24)&0xff, (NetData.ipmask>>16)&0xff,
		(NetData.ipmask>> 8)&0xff,  NetData.ipmask&0xff,
		(NetData.iprouter>>24)&0xff, (NetData.iprouter>>16)&0xff,
		(NetData.iprouter>> 8)&0xff, NetData.iprouter&0xff);

	return;
}
