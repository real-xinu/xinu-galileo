/* colon2mac.c - colon2mac */

#include <xinu.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 * colonmac  -  Parse a colon-hex Ethernet address and convert to binary
 *------------------------------------------------------------------------
 */
int32	colon2mac (
	  char	*src,		/* Ptr to Ethernet address in ASCII of	*/
				/*   the form:  xx:xx:xx:xx:xx:xx	*/
	  byte	*dst		/* Where to put binary form of address	*/
	)
{
	int32	cnt;		/* Count of output bytes		*/
	int32	digit = 0;	/* Next digit in binary			*/
	int32	ch = 0;		/* Next digit in ASCII			*/

	/* Validate arguments */

	if (src == NULL || dst == NULL) {
		return SYSERR;
	}

	/* For each of the six bytes in an Ethernet address */

	for (cnt = 0; (cnt < ETH_ADDR_LEN) && (src != NULLCH); cnt++) {

		/* Get first hex character and convert to binary */

		ch = *src++;
		if (isdigit(ch)) {
			digit = ch - '0';
		} else if (isxdigit(ch)) {
			digit = 10 + ch - (isupper(ch) ? 'A' : 'a');
		} else {
			digit = 0;
		}
		dst[cnt] = digit << 4;

		/* Get second hex character and convert to binary */

		ch = *src++;

		if (isdigit(ch)) {
			digit = ch - '0';
		} else if (isxdigit(ch)) {
			digit = 10 + ch - (isupper(ch) ? 'A' : 'a');
		} else {
			digit = 0;
		}
		dst[cnt] |= digit;

		if (*src++ != ':') {
			break;
		}
	}
	return cnt;
}
