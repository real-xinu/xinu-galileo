/* netstart.c - netstart */

#include <xinu.h>

/*------------------------------------------------------------------------
 * netstart  -   run DHCP, obtain an IP address, and print network info
 *
 * Note: this function cannot be run during initialization; it must be
 *		called from main or a process that can block
 *
 *------------------------------------------------------------------------
 */

void	netstart (void)
{
	uint32	ipaddr;		/* Computer's IP address in binary	*/
	char	str[128];	/* String used to format the output	*/


	/* Use DHCP to obtain an IP address */

	ipaddr = getlocalip();
	if ((int32)ipaddr == SYSERR) {
		return;
	}

	/* Print the IP in dotted decimal and hex */

	ipaddr = NetData.ipucast;
	sprintf(str, "%d.%d.%d.%d",
		(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
		(ipaddr>>8)&0xff,        ipaddr&0xff);
	printf("Obtained IP address  %s   (0x%08x)\n", str, ipaddr);
	return;
}
