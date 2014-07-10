/* xsh_ipaddr.c - xsh_ipaddr */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_ipaddr - obtain and print the IP address, subnet mask and default
 *			router address for each interface that's up
 *------------------------------------------------------------------------
 */
shellcmd xsh_ipaddr(int nargs, char *args[]) {

	int32	iface;			/* inferface number		*/
	struct	ifentry	*ifptr;		/* ptr to interface table entry	*/
	uint32	ipaddr;			/* an IP address in binary	*/
	uint32	ipbcast;		/* IP broadcast addr. in binary	*/
	uint32	ipprefix;		/* IP network prefix in binary	*/
	uint32	router;			/* address of router in binary	*/
	char	str[40];		/* temporary used for formatting*/
	uint32	ipmask;			/* subnet mask in binary	*/

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays IP address information\n");
		printf("Options:\n");
		printf("\t-f\tforce a new DHCP request for interface 0\n");
		printf("\t--help\tdisplay this help and exit\n");
		return OK;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return SYSERR;
	}

	if (nargs == 2) {
		if (strncmp(args[1], "-f", 3) != 0) {
			fprintf(stderr, "%s: invalid argument\n", args[0]);
			fprintf(stderr,
				"Try '%s --help' for more information\n",
				args[0]);
			return 1;
		}
		if ( (if_tab[0].if_state != IF_UP) ||
					(host && (ifprime != 0)) ) {
			fprintf(stderr, "%s: -f invalid (needs iface 0)\n",
							args[0]);
			return 1;
		}
		if_tab[0].if_ipvalid = FALSE;
	}

	/* Verify that at least one network interface is "up" */

	if (ifprime < 0) {
		fprintf(stderr, "%s: no network interface is up\n",
								args[0]);
		return 1;
	}

	for (iface=0; iface<NIFACES; iface++) {
		ifptr = &if_tab[iface];
		if (ifptr->if_state != IF_UP) {
		continue;
		}
		printf("\nFor interface %d:\n", iface);

		/* IP unicast address in dotted decimal and hex */

		ipaddr = ifptr->if_ipucast;
		sprintf(str, "%d.%d.%d.%d",
			(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,        ipaddr&0xff);
		printf("   %-15s  %-15s  0x%08x\n",
			"IP address:", str, ipaddr);

		/* IP network broadcast address in dotted decimal & hex	*/

		ipbcast = ifptr->if_ipbcast;
		sprintf(str, "%d.%d.%d.%d",
			(ipbcast>>24)&0xff, (ipbcast>>16)&0xff,
			(ipbcast>>8)&0xff,        ipbcast&0xff);
		printf("   %-15s  %-15s  0x%08x\n",
			"IP broadcast:", str, ipbcast);

		/* IP network prefix in dotted decimal & hex */

		ipprefix = ifptr->if_ipprefix;
		sprintf(str, "%d.%d.%d.%d",
			(ipprefix>>24)&0xff, (ipprefix>>16)&0xff,
			(ipprefix>>8)&0xff,        ipprefix&0xff);
		printf("   %-15s  %-15s  0x%08x\n",
			"IP prefix:", str, ipprefix);

		/* IP network mask in dotted decimal & hex */

		ipmask = ifptr->if_ipmask;
		ipaddr = ifptr->if_ipucast;
		sprintf(str, "%d.%d.%d.%d",
			(ipmask>>24)&0xff, (ipmask>>16)&0xff,
			(ipmask>>8)&0xff,        ipmask&0xff);
		printf("   %-15s  %-15s  0x%08x\n",
			"Address mask:", str, ipmask);

		/* Default router in dotted decimal & hex */

		router = ifptr->if_iprouter;
		sprintf(str, "%d.%d.%d.%d",
			(router>>24)&0xff, (router>>16)&0xff,
			(router>>8)&0xff,        router&0xff);
		printf("   %-15s  %-15s  0x%08x\n",
			"IP router:", str, router);


		printf("   %-15s  %02x:%02x:%02x:%02x:%02x:%02x\n",
		"MAC unicast:",
		0xff & ifptr->if_macucast[0],
		0xff & ifptr->if_macucast[1],
		0xff & ifptr->if_macucast[2],
		0xff & ifptr->if_macucast[3],
		0xff & ifptr->if_macucast[4],
		0xff & ifptr->if_macucast[5]);

		printf("   %-15s  %02x:%02x:%02x:%02x:%02x:%02x\n",
		"MAC broadcast:",
		0xff & ifptr->if_macbcast[0],
		0xff & ifptr->if_macbcast[1],
		0xff & ifptr->if_macbcast[2],
		0xff & ifptr->if_macbcast[3],
		0xff & ifptr->if_macbcast[4],
		0xff & ifptr->if_macbcast[5]);
	}
	printf("\n");
	return OK;
}
