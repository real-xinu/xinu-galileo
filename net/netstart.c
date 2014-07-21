/* netstart.c - netstart */

#include <xinu.h>

/*------------------------------------------------------------------------
 * netstart  -  Initialize network and run DHCP to get an IP address
 *------------------------------------------------------------------------
 */

void	netstart (void)
{
	uint32	ipaddr;			/* IP address			*/

	/* Initialize the network stack */

	kprintf("...initializing network stack\n");
	net_init();

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
