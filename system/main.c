/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;

process	main(void)
{
	/* Start the network */

	netstart();

	nsaddr = 0x800a0c10;

	tcp_init();

	return OK;
}
