/* namInit.c - namInit */

#include <xinu.h>

#ifndef	RFILESYS
#define	RFILESYS	SYSERR
#endif

#ifndef	FILESYS
#define	FILESYS		SYSERR
#endif

#ifndef	LFILESYS
#define	LFILESYS	SYSERR
#endif

struct	nmentry	nametab[NNAMES];	/* table of name mappings	*/
int32	nnames;				/* num. of entries allocated	*/

/*------------------------------------------------------------------------
 *  namInit  -  initialize the syntactic namespace
 *------------------------------------------------------------------------
 */
status	namInit(void)
{
	did32	i;			/* index into devtab		*/
	struct	dentry *devptr;		/* ptr to device table entry	*/
	char	tmpstr[NM_MAXLEN];	/* string to hold a name	*/
	status	retval;			/* return value			*/
	char	*tptr;			/* ptr into tempstring		*/
	char	*nptr;			/* ptr to device name		*/
	char	devprefix[] = "/dev/";	/* prefix to use for devices	*/
	int32	len;			/* length of created name	*/
	char	ch;			/* storage for a character	*/

	/* Set prefix table to empty */

	nnames = 0;

	for (i=0; i<NDEVS ; i++) {
		tptr = tmpstr;
		nptr = devprefix;

		/* copy prefix into tmpstr*/

		len = 0;
		while ((*tptr++ = *nptr++) != NULLCH) {
			len++;
	        }
                tptr--; /* move pointer to position before NULLCH */
		devptr = &devtab[i];
		nptr = devptr->dvname;	/* move to device name */

		/* map device name to lower case and append */

		while(++len < NM_MAXLEN) {
			ch = *nptr++;
			if ( (ch >= 'A') && (ch <= 'Z')) {
				ch += 'a' - 'A';
			}
			if ( (*tptr++ = ch) == NULLCH) {
				break;
			}
		}

		if (len > NM_MAXLEN) {
			kprintf("namespace: device name %s too long\r\n",
					devptr->dvname);
			continue;
		}

		retval = mount(tmpstr, NULLSTR, devptr->dvnum);
                if (retval == SYSERR) {
			kprintf("namespace: cannot mount device %d\r\n",
					devptr->dvname);
			continue;
		}
	}

	/* Add other prefixes (longest prefix first) */
        mount("/dev/null",	"",        NULLDEV);
	mount("/remote/",	"remote:", RFILESYS);
	mount("/local/",	NULLSTR,   LFILESYS);
	mount("/dev/",		NULLSTR,   SYSERR);
        mount("~/",		NULLSTR,   LFILESYS);
	mount("/",		"root:",   RFILESYS);
	mount("",		"",        LFILESYS);

	return OK;
}
