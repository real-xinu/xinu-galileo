/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

int32	main(void)

{
	uint32	retval;
//	byte	mac1[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
//	char	buf[1500];
//	struct	netpacket pkt;
//	int32	i;
//	bool8	done;

	kprintf("\n\n###########################################################\n\n");

//	done = FALSE;
//	while (!done) {
//		printf("Enter s for sender or r for receiver: ");
//		retval = read(CONSOLE, buf, 20);
//		if ( retval != 2 ) {
//			continue;
//		}
//		if ( (buf[0] != 'r') && (buf[0] != 's') ) {
//			continue;
//		} else {
//			done = TRUE;
//		}
//	}
//	e1000e_rar_set(&ethertab[0], mac1, 4);
//	if ( buf[0] == 'r' ) {
//		while (TRUE) {
//			retval = read(ETHER0, buf, 1500);
//			if (retval < 0) {
//				continue;
//			}
//			if (buf[0] == mac1[0]) {
//				kprintf("success!\n");
//			}
//		}
//	} else {
//		for (i=0; i<1500; i++) {
//			*(i + (char *)&pkt) = 0xff&i;
//		}
//		memcpy(pkt.net_ethdst, mac1, 6);
//		control(ETHER0, ETH_CTRL_GET_MAC,
//				(int32)&pkt.net_ethsrc, 0);
//		pkt.net_ethtype = ntohs(0x0800);
//		while (TRUE) {
//			retval = read(CONSOLE, buf, 100);
//			write(ETHER0, (char *)&pkt, 1200);
//		}
//	}
			
	//kprintf("...starting the network\n");
	netstart();

	while(1) {
		kprintf("clktime %d\n", clktime);
		sleep(1);
	}

	kprintf("\n...creating a shell\n");
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		retval = receive();
		sleepms(200);
		kprintf("\n\n\rMain process recreating shell\n\n\r");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
}
