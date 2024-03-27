/* xsh_memdump.c - xsh_memdump */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

static	uint32	parseval(char *);
extern	uint32	start;

/*------------------------------------------------------------------------
 * xsh_memdump - dump a region of memory by displaying values in hex
 *			 and ascii
 *------------------------------------------------------------------------
 */
shellcmd xsh_memdump(int nargs, char *args[])
{
	bool8	force = FALSE;		/* Ignore address sanity checks	*/
	uint32	begin;			/* Begining address		*/
	uint32	stop;			/* Last address to dump		*/
	uint32	length;			/* Length of region to dump	*/
	int32	arg;			/* Index into args array	*/
	uint32	l;			/* Counts length during dump	*/
	int32	i;			/* Counts words during dump	*/
	uint32	*addr;			/* Address to dump		*/
	char	*chptr;			/* Character address to dump	*/
	char	ch;			/* Next character to print	*/

	/* For argument '--help', emit help about the 'memdump' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [-f] Address Length\n\n", args[0]);
		printf("Description:\n");
		printf("\tDumps Length bytes of memory begining at the\n");
		printf("\tspecified starting address (both the address\n");
		printf("\tand length can be specified in decimal or hex)\n");
		printf("Options:\n");
		printf("\t-f         ignore sanity checks for addresses\n");
		printf("\tAddress    memory address at which to start\n");
		printf("\tLength     the number of bytes to dump\n");
		printf("\t--help     display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs < 3 || nargs > 4) {
		fprintf(stderr, "%s: incorrect number of arguments\n",
				args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	arg = 1;
	if (strncmp(args[arg], "-f", 2) == 0) {
		force = TRUE;
		arg++;
		nargs --;
	}

	if (nargs != 3) {
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if ( (begin=parseval(args[arg])) == 0 ) {
		fprintf(stderr, "%s: invalid begining address\n",
				args[0]);
		return 1;
	}
	if ( (length =parseval(args[arg+1])) == 0 ) {
		fprintf(stderr, "%s: invalid length address\n",
				args[0]);
		return 1;
	}

	/* Round begining address down to multiple of four and round	*/
	/*	length up to a multiple of four				*/

	begin &= ~0x3;
	length = (length + 3) & ~0x3;

	/* Add length to begin address */

	stop = begin + length;

	/* Verify that the address and length are reasonable */

	if ( force || ( (begin >= (uint32)&start) && (stop > begin) &&
					(((void *)stop) < maxheap)) ) {

		/* Values are valid; perform dump */

		chptr = (char *)begin;
		for (l=0; l<length; l+=16) {
			printf("%08x: ", begin);
			addr = (uint32 *)begin;
			for (i=0; i<4; i++) {
				printf("%08x ",*addr++);
			}
			printf("  *");
			for (i=0; i<16; i++) {
				ch = *chptr++;
				if ( (ch >= 0x20) && (ch <= 0x7e) ) {
					printf("%c",ch);
				} else {
					printf(".");
				}
			}
			printf("*\n");
			begin += 16;
		}
		return 0;
	} else {
		printf("Values are out of range; use -f to force\n");
	}
	return 1;
}

/*------------------------------------------------------------------------
 * parse - parse an argument that is either a decimal or hex value
 *------------------------------------------------------------------------
 */
static	uint32	parseval(
	  char	*string			/* Argument string to parse	*/
	)
{
	uint32	value;			/* Value to return		*/
	char	ch;			/* Next character		*/
	

	value = 0;

	/* Argument string must consists of decimal digits or	*/
	/*	0x followed by hex digits			*/

	ch = *string++;
	if (ch == '0') {		/* Hexadecimal */
		if (*string++ != 'x') {
			return 0;
		}
		for (ch = *string++; ch != NULLCH; ch = *string++) {
			if ((ch >= '0') && (ch <= '9') ) {
				value = 16*value + (ch - '0');
			} else if ((ch >= 'a') && (ch <= 'f') ) {
				value = 16*value + 10 + (ch - 'a');
			} else if ((ch >= 'A') && (ch <= 'F') ) {
				value = 16*value + 10 + (ch - 'A');
			} else {
				return 0;
			}
		}
	} else {			/* Decimal */
		while (ch != NULLCH) {
			if ( (ch < '0') || (ch > '9') ) {
				return 0;
			}
			value = 10*value + (ch - '0');
			ch = *string++;
		}
	}
	return value;
}
