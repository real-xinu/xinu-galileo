/* xsh_bingid.c - xsh_bingid */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

extern	int32	bingid;

/*------------------------------------------------------------------------
 * xsh_bingid - shell command to print the current bingid
 *------------------------------------------------------------------------
 */
shellcmd xsh_bingid(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'bingid' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tPrint the current bingid\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs != 1) {
		fprintf(stderr, "%s: invalid arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}
	printf("%d\n", bingid);
	return 0;
}
