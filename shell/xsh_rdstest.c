/* xsh_rdstest.c - xsh_rdstest */
#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_rdstest - shell to print the time the system has been up
 *------------------------------------------------------------------------
 */
shellcmd xsh_rdstest(int nargs, char *args[])
{
	bool8	err;
	int32	retval;
	int32	i, j;
	char	dskname[] = "TestDisk";

	char	buff[2048];
	char	buff2[2048];

	retval = open(RDISK,dskname,"rw");

	if (retval < 0) {
		kprintf("xsh_rdstest: open %s returns %s, stopping\n",
			dskname, retval==SYSERR?"SYSERR":"TIMEOUT");
		close( RDISK );
		return 1;
	} else {
		kprintf("xsh_rdstest: open %s returns %d\n",
			dskname, retval);
	}

	kprintf("writing eight blocks to the disk\n");
	for (i=7; i>=0; i--) {
		memset(buff, (char)(i&0xff), RD_BLKSIZ);
		kprintf("\n*** writing block %d\n",i);
		retval = write(RDISK, buff, i);
		if (retval < 0) {
		   kprintf("write to block %d returns %d\n", i, retval);
		}
	}
	kprintf("reading block 1\n");
	retval = read(RDISK, buff, 1);
	kprintf("read from block 1 has return value %d\n", retval);

	err = 0;
	for (i=0; i<RD_BLKSIZ; i++) {
		if (buff[i] != (char) (0xff&1)) {
			err = 1;
			break;
		}
	}
	if (err == 0) {
		kprintf("Block 1 came back intact!!!\n");
	} else {
		kprintf("Sadly  :-( byte %d is not correct!!!\n", i);
	}

	kprintf("reading block 6\n");
	retval = read(RDISK, buff, 6);
	err = 0;
	for (i=0; i<RD_BLKSIZ; i++) {
		if (buff[i] != (char) (0xff&6)) {
			err = 1;
			break;
		}
	}
	if (err == 0) {
		kprintf("Block 6 came back intact!!!\n");
	} else {
		kprintf("Sadly  :-( byte %d is not correct!!!\n", i);
	}

	j = 0;
	for (i=0; i<RD_BLKSIZ; i++) {
		buff2[i] = "abcdefghijklmnopqrstuvwxyz"[j++];
		j %= 13;
	}

	kprintf("rewriting block 5\n");
	retval = write(RDISK, buff2, 5);
	kprintf("write to block 5 has return value %d\n", retval);

	kprintf("reading block 5\n");
	retval = read(RDISK, buff2, 5);

	err = 0;
	for (i=0; i<RD_BLKSIZ; i++) {
		if (buff2[i] != buff[i]) {
			err = 1;
			break;
		}
	}

	if (err == 0) {
		kprintf("Block 5 came back intact!!!\n");
	} else {
		kprintf("Sadly  :-( byte %d is not correct!!!\n", i);
	}

	memset(buff, NULLCH, RD_BLKSIZ);

	kprintf("reading block 6 again\n");
	retval = read(RDISK, buff, 6);
	kprintf("read from block 6 has return value %d\n", retval);

	err = 0;
	for (i=0; i<RD_BLKSIZ; i++) {
		if ( buff[i] != (char)(0xff&6) ) {
	 		err = 1;
			break;
		}
	}
	if (err == 0) {
		kprintf("Got back identical results!!!\n");
	} else {
		kprintf("Sadly  :-( byte %d differs!!!\n", i);
	}
	return 0;
}
