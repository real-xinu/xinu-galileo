/* tcpparse.c  -  tcpparse */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  tcpparse  -  parse AP:IP:port and set IP address and port variables
 *			Example:   "active:128.10.3.101:80"
 *------------------------------------------------------------------------
 */
int32	tcpparse(
	  char		*pstr,		/* Ptr to arg string		*/
	  uint32	*pipaddr,	/* holds IP address		*/
	  uint16	*pport,		/* Holds port number		*/
	  int32		*active		/* Ptr to active/passive value	*/
	)
{
	char		*ipstart;	/* Start of IP address		*/
	char		ch;		/* One character from the string*/
	int32		i;		/* Index used during copy	*/
	char		tmpstr[20];	/* Temporary to hold IP portion	*/
	uint32		port;		/* Port value during calculation*/

	/* Must start with "active" or "passive" */

	if (*pstr == 'a') {
			*active = 1;
	} else if (*pstr == 'p') {
			*active = 0;
	} else {
			return SYSERR;
	}

	/* Skip active/passive until first colon or end of string */

	while ((*pstr != ':') && (*pstr != NULLCH) ) {
		pstr++;
	}

	if (*pstr == NULLCH) {
		return SYSERR;
	} else {
		pstr++;
	}

	/* Record start of IP address portion */

	ipstart = pstr;

	while ((*pstr != ':') && (*pstr != NULLCH) ) {
		pstr++;
	}

	if ( (*pstr == NULLCH) || ((pstr-ipstart) > 16) ) {
		return SYSERR;
	}

	/* copy IP address portion to tmpstr */

	i=0;
	while (ipstart != pstr) {
		tmpstr[i++] = *ipstart++;
	}
	tmpstr[i] = NULLCH;
	if ( dot2ip(tmpstr, pipaddr) == SYSERR ) {
		return SYSERR;
	}

	/* Move past ':' and scan port value */

	pstr++;

	port = 0;
	while (*pstr != NULLCH) {
		ch = *pstr;
		if ( (ch < '0') || (ch > '9') ) {
			return SYSERR;
		}
		port = 10*port + (ch - '0');
		pstr++;
	}
	if ( (port>>16) != 0 ) {
		return SYSERR;
	} else {
		*pport = port;
	}
	return OK;
}
