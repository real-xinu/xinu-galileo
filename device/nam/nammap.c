/* nammap.c - nammap, namrepl, namcpy */

#include <xinu.h>

status	namcpy(char *, char *, int32);
did32	namrepl(char *, char[]);

/*------------------------------------------------------------------------
 *  nammap  -  using namespace, map name to new name and new device 
 *------------------------------------------------------------------------
 */
devcall	nammap(
	 char	*name,			/* a name to map		*/
	 char	newname[NM_MAXLEN],	/* buffer for mapped name	*/
	 did32 namdev			/* ID of the namespace device	*/
	)
{
	did32	newdev;			/* device descriptor to return	*/
	char	tmpname[NM_MAXLEN];	/* temporary buffer for name	*/
	int32	iter;			/* number of iterations		*/

	/* place original name in temporary buffer and null terminate */

	if (namcpy(tmpname, name, NM_MAXLEN) == SYSERR) {
		return SYSERR;
	}

	/* repeatedly substitute the name prefix until a non-namespace	*/
	/* device is reached or an iteration limit is exceeded		*/

	for (iter=0; iter<nnames ; iter++) {
		newdev = namrepl(tmpname, newname);
		if (newdev != namdev) {
                        namcpy(tmpname, newname, NM_MAXLEN);
			return newdev;	/* either valid ID or SYSERR	*/
		}
	}
	return SYSERR;
}

/*------------------------------------------------------------------------
 *  namrepl  -  use the name table to perform prefix substitution
 *------------------------------------------------------------------------
 */
did32	namrepl(
	 char	*name,			/* original name		*/
	 char	newname[NM_MAXLEN]	/* buffer for mapped name	*/
	)
{
	int32	i;			/* iterate through name table	*/
	char	*pptr;			/* walks through a prefix	*/
	char	*rptr;			/* walks through a replacement	*/
	char	*optr;			/* walks through original name	*/
        char    *nptr;                  /* walks through new name       */
	char	olen;			/* length of original name	*/
					/*  including the NULL byte	*/
	int32	plen;			/* length of a prefix string	*/
					/*  *not* including NULL byte	*/
	int32	rlen;			/* length of replacment string	*/
	int32	remain;			/* bytes in name beyond prefix	*/
	struct	nmentry	*namptr;	/* pointer to a table entry	*/

	/* search name table for first prefix that matches */

	for (i=0; i<nnames; i++) {
		namptr = &nametab[i];
		optr = name;		/* start at beginning of name	*/
		pptr = namptr->nprefix;	/* start at beginning of prefix	*/
		
		/* compare prefix to string and count prefix size */

		for (plen=0; *pptr != NULLCH ; plen++) {
			if (*pptr != *optr) {
				break;
			}
			pptr++;
			optr++;
		}
		if (*pptr != NULLCH) {	/* prefix does not match */
			continue;
		}

		/* Found a match - check that replacement string plus	*/
		/* bytes remaining at the end of the original name will	*/
		/* fit into new name buffer.  Ignore null on replacement*/
		/* string, but keep null on remainder of name.		*/

		olen = namlen(name ,NM_MAXLEN);
		rlen = namlen(namptr->nreplace,NM_MAXLEN) - 1;
		remain = olen - plen;
		if ( (rlen + remain) > NM_MAXLEN) {
			return (did32)SYSERR;
		}

		/* place replacement string followed by remainder of */
		/* original name (and null) into the new name buffer */

		
                nptr = newname;
                rptr = namptr->nreplace;
		for (; rlen>0 ; rlen--) {
			*nptr++ = *rptr++;
		}
		for (; remain>0 ; remain--) {
			*nptr++ = *optr++;
		}
		return namptr->ndevice;
	}
	return (did32)SYSERR;
}

/*------------------------------------------------------------------------
 *  namcpy  -  copy a name from one buffer to another, checking length
 *------------------------------------------------------------------------
 */
status	namcpy(
	 char		*newname,	/* buffer to hold copy		*/
	 char		*oldname,	/* buffer containing name	*/
    	 int32		buflen		/* size of buffer for copy	*/
	)
{
        char    *nptr;                  /* point to new name            */
        char    *optr;                  /* point to old name            */
	int32	cnt;			/* count of characters copied	*/

        nptr = newname;
        optr = oldname;

	for (cnt=0; cnt<buflen; cnt++) {
		if ( (*nptr++ = *optr++) == NULLCH) {
			return OK;
		}
	}
	return SYSERR;		/* buffer filled before copy completed	*/
}
