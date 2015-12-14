/* xsh_base64.c - xsh_base64 */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_base64 - base64 encode/decode stdin and print it to stdout
 *------------------------------------------------------------------------
 */
shellcmd xsh_base64(int nargs, char *args[])
{
	int32 block = 3;
	byte in[4];
	byte out[4];
	int32 i, nextch;

	if (nargs > 2 || ((nargs == 2) && strncmp(args[1], "--help", 7) == 0)) {
		printf("Use: %s [-d]\n\n", args[0]);
		printf("Description:\n");
		printf("\tbase64 encode/decode stdin to stdout\n");
		printf("Options:\n");
		printf("\t-e\tencode\n");
		printf("\t-d\tdecode\n");
		printf("\t-d\tdecode\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	if (nargs == 2) {
		if (strncmp(args[1], "-d", 2) == 0) {
			block = 4;
		} else if (strncmp(args[1], "-e", 2) == 0) {
			block = 3;
		} else {
			printf("invalid option. (See --help)\n");
			return 1;
		}
	}

	nextch = 0;
	while (TRUE) {

		in[nextch] = getc(stdin);
		if (in[nextch++] == EOF) {
			break;
		}
		if (nextch == block) {
			if (block == 4) {
				nextch = base64_decode(in, block, out);
			} else {
				nextch = base64_encode(in, block, out);
			}
			for (i = 0 ; i < nextch ; i++) {
				putc(stdout, out[i]);
			}
			nextch = 0;
		}
	}
	if (block == 4) {
		nextch = base64_decode(in, nextch, out);
	} else {
		nextch = base64_encode(in, nextch, out);
	}
	for (i = 0 ; i < nextch ; i++) {
		putc(stdout, out[i]);
	}

	return 0;
}
