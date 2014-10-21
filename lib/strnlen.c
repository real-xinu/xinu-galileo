/* strnlen.c - strnlen */

/*------------------------------------------------------------------------
 *  strnlen  -  Counts the number of non-NULL bytes in a string or len.
 *			    Return length of the string up to user specified length.
 *------------------------------------------------------------------------
 */
int		strnlen(
		  const char		*s,			/* string						*/
		  unsigned int		len			/* length you wish to scan s to	*/
		)
{
    int n;

    n = 0;
    while (*s++ && n < len)
        n++;

    return (n);
}
