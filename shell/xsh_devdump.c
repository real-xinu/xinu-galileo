/* xsh_devdump.c  - xsh_devdump */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_devdump - shell command to print info from the device switch table
 *------------------------------------------------------------------------
 */
shellcmd xsh_devdump (
	 int	nargs,			/* Number of arguments		*/
	 char	*args[]			/* List of arguments		*/
 	)
{
	struct	dentry	*devptr;	/* Pointer to device entry	*/
	int32	i;			/* Walks through device table	*/

	/* No arguments are expected */

	if (nargs > 1) {
		fprintf(stderr, "No arguments allowed\n");
		return 1;
	}

	/* Walk through device table */

	printf("Device     Name     Minor\n");
	printf("------ ------------ -----\n");

	for (i = 0; i < NDEVS; i++) {
		devptr = &devtab[i];
		printf("%4d   %-12s %3d\n", i, devptr->dvname,
			devptr->dvminor);
	}
	return 0;
}
