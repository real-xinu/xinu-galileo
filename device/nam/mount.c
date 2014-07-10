/* mount.c - mount, namlen */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  mount  -  Add a prefix mapping to the name space
 *------------------------------------------------------------------------
 */
syscall	mount(
	  char		*prefix,	/* prefix to add		*/
	  char		*replace,	/* replacement string		*/
	  did32		device		/* device ID to use		*/
)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	nmentry	*namptr;	/* pointer to unused table entry*/
	int32	psiz, rsiz;		/* sizes of prefix & replacement*/
	int32	i;			/* counter for copy loop	*/

        mask = disable();

	psiz = namlen(prefix, NM_PRELEN);
	rsiz = namlen(replace, NM_REPLLEN);
	if ((psiz == SYSERR) || (rsiz == SYSERR) || isbaddev(device)) {
		restore(mask);
		return SYSERR;
	}

	if (nnames >= NNAMES) {		/* if table full return error */
		restore(mask);
		return SYSERR;
	}

	/* allocate a slot in the table */

	namptr = &nametab[nnames];	/* next unused entry in table	*/

	/* copy prefix and replacement strings and record device ID */
	
	for (i=0; i<psiz; i++) {	/* copy prefix into table entry	*/
		namptr->nprefix[i] = *prefix++;
	}

	for (i=0; i<rsiz; i++) {	/* copy replacement into entry	*/
		namptr->nreplace[i] = *replace++;
	}

	namptr->ndevice = device;	/* record the device ID		*/

        nnames++;			/* increment number of names	*/

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *  namlen  -  compute the length of a string stopping at maxlen
 *------------------------------------------------------------------------
 */
int32	namlen(
	  char		*name,		/* name to use			*/
	  int32		maxlen		/* maximum length (including a	*/
					/*   NULL byte)			*/
)
{
	int32	i;			/* counter */

	/* search until a null terminator or length reaches max */

	for (i=0; i < maxlen; i++) {
		if (*name++ == NULLCH) {
			return i+1;
		}
	}
	return SYSERR;
}
