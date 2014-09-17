/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

extern uint32 nsaddr;

process	main(void)
{
	/* Start the network */

	netstart();

	nsaddr = 0x800a0c10;

	kprintf("rfs test 1 : open more than 10 files\n");
	did32 files[15];
	int32 i;
	for(i = 0; i < 10; i++) {
		char fname[50];
		sprintf(fname, "testfile%d", i);
		files[i] = open(RFILESYS, fname, "w");
	}
	files[10] = open(RFILESYS, "testfile10", "w");
	kprintf("11th file: %d\n", files[10]);
	if((int32)files[10] == SYSERR) {
		kprintf("Test passed\n");
	}
	else {
		kprintf("test failed\n");
	}

	for(i = 0; i <10; i++) {
		close(files[i]);
	}

	char buf[100] = {'\0'};
	files[0] = open(RFILESYS, "testfile0", "rw");
	write(files[0], "this is write and read test", 27);
	read(files[0], buf, 10);
	kprintf("read from the file: %s\n", buf);

	kprintf("\n...creating a shell\n");
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
}
