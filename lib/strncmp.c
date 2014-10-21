/* strncmp.c - strncmp */

/*------------------------------------------------------------------------
 *  strncmp  -  Compare strings (at most n bytes).
 *				Return s1>s2: >0	s1==s2: 0	s1<s2: <0
 *------------------------------------------------------------------------
 */
int		strncmp(
		  char		*s1,		/* first memory location				*/
		  char		*s2,		/* second memory location				*/
		  int		n			/* length to compare					*/
		)
{

    while (--n >= 0 && *s1 == *s2++)
    {
        if (*s1++ == '\0')
        {
            return 0;
        }
    }
    return (n < 0 ? 0 : *s1 - *--s2);
}
