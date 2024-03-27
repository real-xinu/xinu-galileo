/* xsh_tee.c - xsh_tee */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_tee - shell command to copy stdin to a file and to stdout
 *------------------------------------------------------------------------
 */
shellcmd xsh_tee(int nargs, char *args[])
{
	int32	nextch;			/* Character read from file	*/
	did32	descr;			/* Descriptor for a file	*/


	/* For argument '--help', emit help about the 'tee' command	*/

	if (nargs != 2 && strncmp(args[1], "--help", 7) == 0) {
		fprintf(stderr,"Use: %s file\n\n", args[0]);
		fprintf(stderr,"Description:\n");
		fprintf(stderr,"\tcopies stdin to stdout and a file\n");
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t--help\t display this help and exit\n");
		return 0;
	}

	descr = open(NAMESPACE, args[1], "w");
	if (descr == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot open file %s\n",
			args[0], args[1]);
			return 1;
	}
	control(descr, F_CTL_TRUNC, 0, 0);
	nextch = getc(stdin);
	while (nextch != EOF) {
		putc(stdout, nextch);
		putc(descr, nextch);
		nextch = getc(stdin);
	}
	close(descr);
	return 0;
}




