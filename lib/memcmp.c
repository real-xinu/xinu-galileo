/* memcmp.c - memcmp */

/*------------------------------------------------------------------------
 *  memcmp  -  Compare memory (ISO C89).
 *			   Assumes memory locations are same length.
 *			   Return s1>s2: >0		s1==s2: 0		s1<s2: <0
 *------------------------------------------------------------------------
 */
int		memcmp(
		  const void		*s1,		/* first memory location		*/
		  const void		*s2,		/* second memory location		*/
		  int				n			/* length to compare			*/
		)
{
    const unsigned char *c1;
    const unsigned char *c2;

    for (c1 = s1, c2 = s2; n > 0; n--, c1++, c2++)
    {
        if (*c1 != *c2)
        {
            return ((int)*c1) - ((int)*c2);
        }
    }
    return 0;
}
