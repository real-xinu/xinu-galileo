/* dhcp.c - getlocalip */

#include <xinu.h>
#include <stdlib.h>

/*------------------------------------------------------------------------
 * dump_dhcp_msg - Print out a DHCP message 
 *------------------------------------------------------------------------
 */
void dump_dhcp_msg(const struct dhcpmsg* dmsg, uint32 dmsg_size)
{
	uint32 i,j;
	unsigned char* opt_tmp; /* Pointer to current DHCP option */
	unsigned char* eom;     /* Pointer to the end of the message */

	kprintf("DHCP Message Dump\n");
	kprintf("OP: %d (0x%02X)\n", dmsg->dc_bop, (uint8)dmsg->dc_bop);
	kprintf("HTYPE: %d (0x%02X)\n", dmsg->dc_htype, (uint8)dmsg->dc_htype);
	kprintf("HLEN: %d (0x%02X)\n", dmsg->dc_hlen, (uint8)dmsg->dc_hlen);
	kprintf("HOPS: %d (0x%02X)\n", dmsg->dc_hops, (uint8)dmsg->dc_hops);
	kprintf("XID: %d (0x%08X)\n", ntohl(dmsg->dc_xid), ntohl((uint32)dmsg->dc_xid));
	kprintf("SECS: %d (0x%04X)\n", dmsg->dc_secs, (uint16)dmsg->dc_secs);
	kprintf("FLAGS: %d (0x%04X)\n", dmsg->dc_flags, (uint16)dmsg->dc_flags);
	kprintf("Client IP: %d (0x%08X)\n", ntohl(dmsg->dc_cip), ntohl((uint32)dmsg->dc_cip));
	kprintf("Your IP: %d (0x%08X)\n", ntohl(dmsg->dc_yip), ntohl((uint32)dmsg->dc_yip));
	kprintf("Server IP: %d (0x%08X)\n", ntohl(dmsg->dc_sip), ntohl((uint32)dmsg->dc_sip));
	kprintf("Gateway IP: %d (0x%08X)\n", ntohl(dmsg->dc_gip), ntohl((uint32)dmsg->dc_gip));
	kprintf("Client Hardware Address:\n  ");
	for(i = 0; i < sizeof(dmsg->dc_chaddr); i++) {
		kprintf("%02X", (uint8)dmsg->dc_chaddr[i]);
	}
	kprintf("\n");
	kprintf("BOOTP Server Name: %.64s\n", dmsg->sname);
	kprintf("BOOTP File Name: %.128s\n", dmsg->bootfile);
	kprintf("Magic Cookie: %d (0x%08X)\n", dmsg->dc_cookie, (uint32)dmsg->dc_cookie);

	eom = (unsigned char*)dmsg + dmsg_size - 1;
	opt_tmp = (unsigned char*)dmsg->dc_opt;

	while(opt_tmp < eom) {

		switch(*opt_tmp) {

			case DHCP_PADDING:
			case DHCP_MESSAGE_END:
				opt_tmp++;
				continue;
				break;
				
			case DHCP_SUBNET_MASK:
				kprintf("DHCP Subnet Mask: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;
				
			case DHCP_ROUTER:
				j = *(uint8*)(opt_tmp+1);
				for(i = 0; i < j/4; i++) {
					kprintf("DHCP Router: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2+4*i)), ntohl(*(uint32*)(opt_tmp+2+4*i)));
				}
				break;
			
			case DHCP_DNS_SERVER:
				j = *(uint8*)(opt_tmp+1);
				for(i = 0; i < j/4; i++) {
					kprintf("DHCP DNS Server: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2+4*i)), ntohl(*(uint32*)(opt_tmp+2+4*i)));
				}
				break;

			case DHCP_DOMAIN_NAME:
				kprintf("DHCP Domain Name: ");
				j = *(uint8*)(opt_tmp+1);
				for(i = 0; i < j; i++) {
					kprintf("%c", *((char*)(opt_tmp + 2 + i)));
				}
				kprintf("\n");
				break;
			
			case DHCP_IP_ADDR_LEASE_TIME:
				kprintf("DHCP IP Lease Time: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;
				
			case DHCP_RENEWAL_TIME_VALUE:
				kprintf("DHCP Renewal (T1) Time: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;
			
			case DHCP_REBINDING_TIME_VALUE:
				kprintf("DHCP Rebinding (T2) Time: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;		

			case DHCP_MESSAGE_TYPE:
				kprintf("DHCP Message Type: %d\n", *(uint8*)(opt_tmp+2));
				break;
				
			case DHCP_REQUESTED_IP:
				kprintf("DHCP Requested IP: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;
				
			case DHCP_SERVER_ID:
				kprintf("DHCP Server ID: %d (0x%08X)\n", ntohl(*(uint32*)(opt_tmp+2)), ntohl(*(uint32*)(opt_tmp+2)));
				break;

			case DHCP_VENDER_OPTIONS:
				kprintf("DHCP Vender Options List:\n");
				j = *(uint8*)(opt_tmp+1);
				for(i = 0; i < j; i++) {
					kprintf("  %d\n", *(uint8*)(opt_tmp+i+2));
				}
				break;
				
			case DHCP_PARAMETER_REQUEST_LIST:
				kprintf("DHCP Parameter List:\n");
				j = *(uint8*)(opt_tmp+1);
				for(i = 0; i < j; i++) {
					kprintf("  %d\n", *(uint8*)(opt_tmp+i+2));
				}
				break;
				
			case DHCP_VENDOR_CLASS_ID:
				kprintf("DHCP Class Identifier: %.32s\n", (char*)(opt_tmp+2));
				break;
				
			default:
				kprintf("Unknown DHCP option: %d\n", (unsigned char)(*opt_tmp));
				break;

		}

		opt_tmp++;	/* move to length octet */
	    opt_tmp += *(uint8*)opt_tmp + 1;
	}
	kprintf("\n");
}


/*------------------------------------------------------------------------
 * get_dhcp_option_value - Retrieve a pointer to the value for a particular
 *                           DHCP options key 
 *------------------------------------------------------------------------
 */
char* get_dhcp_option_value(const struct dhcpmsg* dmsg, uint32 dmsg_size, uint8 option_key) 
{
	unsigned char* opt_tmp;
	unsigned char* eom;
	
	eom = (unsigned char*)dmsg + dmsg_size - 1;
	opt_tmp = (unsigned char*)dmsg->dc_opt;
	
	while(opt_tmp < eom) {
	
		/* If the option value matches return the value */
		if((*opt_tmp) == option_key) {
	
			/* Offset past the option value and the size (2 bytes) */
			return (char*)(opt_tmp+2);  
		}
	
		opt_tmp++;	/* move to length octet */
	    opt_tmp += *(uint8*)opt_tmp + 1;
	}
	
	/* Option value not found */
	return NULL;
}

/*------------------------------------------------------------------------
 * build_dhcp_discover - handcraft a DHCP Discover message in dmsg 
 *------------------------------------------------------------------------
 */
int32 build_dhcp_discover(struct dhcpmsg* dmsg) 
{
	uint32  j;
	uint32	xid;			/* xid used for the exchange	*/
		
	memcpy(&xid, NetData.ethucast, 4); /* use 4 bytes from MAC as XID */
	memset(dmsg, 0x00, sizeof(struct dhcpmsg));
	
	dmsg->dc_bop = 0x01;	     	/* Outgoing request		*/
	dmsg->dc_htype = 0x01;		/* hardware type is Ethernet	*/
	dmsg->dc_hlen = 0x06;		/* hardware address length	*/
	dmsg->dc_hops = 0x00;		/* Hop count			*/
	dmsg->dc_xid = htonl(xid);	/* xid (unique ID)		*/
	dmsg->dc_secs = 0x0000;		/* seconds			*/
	dmsg->dc_flags = 0x0000;		/* flags			*/
	dmsg->dc_cip = 0x00000000;	/* Client IP address		*/
	dmsg->dc_yip = 0x00000000;	/* Your IP address		*/
	dmsg->dc_sip = 0x00000000;	/* Server IP address		*/
	dmsg->dc_gip = 0x00000000;	/* Gateway IP address		*/
	memset(&dmsg->dc_chaddr,'\0',16);/* Client hardware address	*/
	memcpy(&dmsg->dc_chaddr, NetData.ethucast, ETH_ADDR_LEN);
	memset(&dmsg->dc_bootp,'\0',192);/* zero the bootp area		*/
	dmsg->dc_cookie = htonl(0x63825363); /* Magic cookie for DHCP	*/
	
	j = 0;
	dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* DHCP Dicover message		*/
	dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

	dmsg->dc_opt[j++] = 0xff & 55;	/* DHCP parameter request list	*/
	dmsg->dc_opt[j++] = 0xff &  2;	/* option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* request subnet mask 		*/
	dmsg->dc_opt[j++] = 0xff &  3;	/* request default router addr->	*/

	return (uint32)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * build_dhcp_request - handcraft a DHCP request message in dmsg 
 *------------------------------------------------------------------------
 */
int32 build_dhcp_request(struct dhcpmsg* dmsg, const struct dhcpmsg* dmsg_offer, uint32 dsmg_offer_size) 
{
	uint32  j;
	uint32	xid;			  /* xid used for the exchange	*/
	uint32* server_ip;        /* DHCP server IP from DHCP offer message */
	
	memcpy(&xid, NetData.ethucast, 4); /* use 4 bytes from MAC as XID */
	memset(dmsg, 0x00, sizeof(struct dhcpmsg));
	
	dmsg->dc_bop = 0x01;	     	/* Outgoing request		*/
	dmsg->dc_htype = 0x01;		/* hardware type is Ethernet	*/
	dmsg->dc_hlen = 0x06;		/* hardware address length	*/
	dmsg->dc_hops = 0x00;		/* Hop count			*/
	dmsg->dc_xid = htonl(xid);	/* xid (unique ID)		*/
	dmsg->dc_secs = 0x0000;		/* seconds			*/
	dmsg->dc_flags = 0x0000;		/* flags			*/
	dmsg->dc_cip = 0x00000000; 	/* Client IP address		*/
	dmsg->dc_yip = 0x00000000;	/* Your IP address		*/
	dmsg->dc_sip = dmsg_offer->dc_sip;	/* Server IP address		*/
	dmsg->dc_gip = 0x00000000;	/* Gateway IP address		*/
	memset(&dmsg->dc_chaddr,'\0',16);/* Client hardware address	*/
	memcpy(&dmsg->dc_chaddr, NetData.ethucast, ETH_ADDR_LEN);
	memset(&dmsg->dc_bootp,'\0',192);/* zero the bootp area		*/
	dmsg->dc_cookie = htonl(0x63825363); /* Magic cookie for DHCP	*/

	j = 0;
	dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* option length		*/
	dmsg->dc_opt[j++] = 0xff &  3;	/* DHCP Request message		*/
	dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

	dmsg->dc_opt[j++] = 0xff & 50;	/* Requested IP	*/
	dmsg->dc_opt[j++] = 0xff &  4;	/* option length		*/
	*((uint32*)&dmsg->dc_opt[j]) = dmsg_offer->dc_yip;
	j += 4;
	
	/* Retrieve the DHCP server IP from the DHCP options */
	server_ip = (uint32*)get_dhcp_option_value(dmsg_offer, dsmg_offer_size, DHCP_SERVER_ID);
	
	if(server_ip == 0) {
		kprintf("Unable to retrieve DHCP server IP address from DHCP Offer\n");
		dump_dhcp_msg(dmsg_offer, dsmg_offer_size);
		return SYSERR;
	}
	
	dmsg->dc_opt[j++] = 0xff & 54;	/* Server IP	*/
	dmsg->dc_opt[j++] = 0xff &  4;	/* option length		*/
	*((uint32*)&dmsg->dc_opt[j]) = *server_ip;
	j += 4;

	return (uint32)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * getlocalip - use DHCP to obtain an IP address
 *------------------------------------------------------------------------
 */
uint32	getlocalip(void)
{
	return getlocalip_boot(NULL, NULL, NULL);
}

/*------------------------------------------------------------------------
 * getlocalip_boot - use DHCP to obtain an IP address
 *------------------------------------------------------------------------
 */
uint32	getlocalip_boot(uint32* boot_server, char* boot_file, uint32* size)
{
	int32	slot;			/* UDP slot to use		*/
	struct	dhcpmsg dmsg_snd;		/* holds outgoing DHCP messages	*/
	struct	dhcpmsg dmsg_rvc;		/* holds incoming DHCP messages	*/

	int32	i, j;			/* retry counters		*/
	int32	len;			/* length of data sent		*/
	int32	inlen;			/* length of data received	*/
	char	*optptr;		/* pointer to options area	*/
	char	*eop;			/* address of end of packet	*/
	int32	msgtype;		/* type of DCHP message		*/
	uint32	addrmask;		/* address mask for network	*/
	uint32	routeraddr;		/* default router address	*/
	uint32	tmp;			/* used for byte conversion	*/
	uint32* tmp_server_ip;  /* temp DHCP server pointer */

	slot = udp_register(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT);
	if (slot == SYSERR) {
		kprintf("getlocalip: cannot register with UDP\r\n");
		return SYSERR;
	}

	len = build_dhcp_discover(&dmsg_snd);
	if(len == SYSERR) {
		kprintf("getlocalip: Unable to build DHCP discover\r\n");
		return SYSERR;
	}

	for (i = 0; i < DHCP_RETRY; i++) {
		udp_sendto(slot, IP_BCAST, UDP_DHCP_SPORT, (char *)&dmsg_snd, len);

		/* Read 3 incoming DHCP messages and check for an offer	*/
		/* 	or wait for three timeout periods if no message */
		/* 	arrives.					*/

		for (j=0; j<3; j++) {
			inlen = udp_recv(slot, (char *)&dmsg_rvc, sizeof(struct dhcpmsg),2000);
			if (inlen == TIMEOUT) {
				continue;
			} else if (inlen == SYSERR) {
				return SYSERR;
			}
			/* Check that incoming message is a valid	*/
			/* response (ID	matches our request)		*/

			if ( (ntohl(dmsg_rvc.dc_xid) != ntohl(dmsg_snd.dc_xid)) ) {
				continue;
			}

			eop = (char *)&dmsg_rvc + inlen - 1;
			optptr = (char *)&dmsg_rvc.dc_opt;
			msgtype = addrmask = routeraddr = 0;
			
			while (optptr < eop) {

				switch (*optptr) {
					case 53:	/* message type */
						msgtype = 0xff & *(optptr+2);
					break;

					case 1:		/* subnet mask */
						memcpy((void *)&tmp, optptr+2, 4);
						addrmask = ntohl(tmp);
					break;

					case 3:		/* router address */
						memcpy((void *)&tmp, optptr+2, 4);
						routeraddr = ntohl(tmp);
						break;
				}
		    	optptr++;	/* move to length octet */
		    	optptr += (0xff & *optptr) + 1;
			}

			if (msgtype == 0x02) {	/* offer - send request	*/
				len = build_dhcp_request(&dmsg_snd, &dmsg_rvc, inlen);
				if(len == SYSERR) {
					kprintf("getlocalip: Unable to build DHCP request\r\n");
					return SYSERR;
				}
				udp_sendto(slot, IP_BCAST, UDP_DHCP_SPORT,
					(char *)&dmsg_snd, len);
				continue;
			
			} else if (dmsg_rvc.dc_opt[2] != 0x05) {
				/* if not an ack skip it */
				continue;
			}
			if (addrmask != 0) {
				NetData.ipmask = addrmask;
			}
			if (routeraddr != 0) {
				NetData.iprouter = routeraddr;
			}
			NetData.ipucast = ntohl(dmsg_rvc.dc_yip);
			NetData.ipprefix = NetData.ipucast & NetData.ipmask;
			NetData.ipbcast = NetData.ipprefix | ~NetData.ipmask;
			NetData.ipvalid = TRUE;
			udp_release(slot);
			
			if(boot_server != NULL && 
			   size != NULL && 
			   boot_file != NULL) {
			   
			    /* Retrieve the boot server IP */
			    if(dot2ip((char*)dmsg_rvc.sname, boot_server) != OK) {
				
					/* Could not retrieve the boot server from the BOOTP fields */
					/*   Assume the boot server is the DHCP server              */
					/* Retrieve the DHCP server IP from the DHCP options */
					tmp_server_ip = (uint32*)get_dhcp_option_value(&dmsg_rvc, len, DHCP_SERVER_ID);
					if(tmp_server_ip == 0) {
						kprintf("Unable to retrieve boot server IP\n");
						dump_dhcp_msg(&dmsg_rvc, len);
						return (uint32)SYSERR;
					}
					(*boot_server) = ntohl(*tmp_server_ip);
				}
				(*size) = (*size) < sizeof(dmsg_rvc.bootfile) ? (*size) : sizeof(dmsg_rvc.bootfile);
				memcpy(boot_file, dmsg_rvc.bootfile, *size);
			}
			
			return NetData.ipucast;
		}
	}

	kprintf("DHCP failed to get response\r\n");
	udp_release(slot);
	return (uint32)SYSERR;
}
