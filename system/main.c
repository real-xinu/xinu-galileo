/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;

process	main(void)
{
	pid32	shell_pid;

	/* need to initialize random */
	init_randompool();

	/* Need to initialize network */
	netstart();
	tcp_init();

	/* Need bufferpool and port mechanism */
	bufinit();
	ptinit(SSH_BUFF_NB);

	kprintf("Start\n");
	if (ssh_init() == FALSE) {
		kprintf("Fail init\n");
	} else {
		resume(create(ssh_deamon, SSH_DEAMON_STACK_SIZE, SSH_DEAMON_PRIO, "ssh-deamon", 0, NULL));
	}

	kprintf("\n...creating a shell\n");
	recvclr();
	resume(shell_pid = create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		while(shell_pid != receive());
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
}
