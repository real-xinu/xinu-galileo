/* xsh_ns.c  - xsh_ns */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_ns - shell command to print info from the device switch table
 *------------------------------------------------------------------------
 */
shellcmd xsh_ns (
	 int	nargs,			/* Number of arguments		*/
	 char	*args[]			/* List of arguments		*/
 	)
{
	int32	i;			/* Walks through the namespace	*/
	struct	nmentry	*nptr;		/* Pointer to a namespace entry	*/
	char	*pptr;			/* Pointer to a prefix		*/
	char	*rptr;			/* Pointer to a replacement	*/
	char	nullstr[] = "NULLSTR";	/* Used when an item is ""	*/
	char	repname[NM_MAXLEN];	/* Buffer to hold a mapped name	*/
	did32	dev;			/* Device for a mapped name	*/
	int32	slen;			/* Length of an argument	*/
	int32	cpylen;			/* Bytes to copy		*/
	int32	d;			/* Walks through devtab		*/

	char	err[] = "invalid arguments - type  --help for details";
	char	*use[] = {
	"With no arguments, ns prints the namespace.  Arguments are",
	"interpreted as follows:",
	"    --help         displays this message.",
	"    -d prefix      removes the prefix from the namspace",
	"    -m ppp rrr ddd mount a namespace entry with prefix ppp,",
	"                     repalcement rrr and device name ddd",
        "    xxx           causes ns to map file name xxx through",
	"                    the namspace and display the result."
	};

	if ( (nargs == 4) || (nargs > 5) ) {

		return 1;
	}

	/* Handle no arguments by dumping the namespace */

	if (nargs == 1) {
		if (nnames <= 0) {
			fprintf(stderr, "namespace is empty!\n");
			return 1;
		}
		printf("%s  %s  %s\n",
			"       Prefix          ",
			"    Replacement        ",
			"      Device       ");
		printf("%s  %s  %s\n",
			"-----------------------",
			"-----------------------",
			"-------------------");
		/* Walk through the namespace */
		for (i=0; i<nnames; i++) {
			nptr = &nametab[i];
			pptr = nptr->nprefix;
			if (*pptr == NULLCH) {
				pptr = nullstr;
			}
			rptr = nptr->nreplace;
			if (*rptr == NULLCH) {
				rptr = nullstr;
			}
			printf("%-24s %-24s %s\n", pptr, rptr,
				devtab[nptr->ndevice].dvname);
		}
		return 0;
	}

	/* Handle the case of one argument by mapping the name */

	if (nargs == 2) {

		if (strncmp(args[1], "--help", 7) == 0) {
			fprintf(stderr, "use:\n  %s\n  %s\n  %s\n  ",
				use[0], use[1], use[2]);
			fprintf(stderr,"%s\n  %s\n  %s\n\n",
				use[3], use[4], use[5]);
			return 0;
		}
		if (args[1][0] == NULLCH) {
			fprintf(stderr, "%s\n", err);
			return 1;
		}
		dev = namrepl(args[1], repname);
		printf(" %s becomes %s on device %d\n",
				args[1], repname, dev);
		return 0;
	}

	/* Handle the -d option */

	if (nargs == 3) {

		if (strncmp(args[1], "-d", 3) != 0) {
			fprintf(stderr, "%s\n", err);
			return 1;
		}
		slen = strlen(args[2]) + 1;
		for (i=0; i<nnames; i++) {
			nptr = &nametab[i];
			if (strncmp(args[2],nptr->nprefix, slen)
								== 0) {
			    cpylen = (nnames-i-1)*sizeof(struct nmentry);
			    if (cpylen > 0) {
				memcpy((void *)nptr,(void *)&nametab[i+1],
					cpylen);
			    }
			    nnames--;
			    return 0;
			}
		}
		fprintf(stderr, "No such entry in the namepace\n");
		return 1;
	}

	/* Handle the -m option */
	if ( strncmp(args[1], "-m", 2) != 0 ) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	slen = strlen(args[4]);
	for (d=0; d < NDEVS; d++) {
		if (strncmp(args[4], devtab[d].dvname, slen) == 0) {
			mount(args[2], args[3], d);
			return 0;
		}
	}
	fprintf(stderr, "invalid device name\n");
	return 1;
}
