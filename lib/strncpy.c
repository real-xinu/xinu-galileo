/* strncpy.c - strncpy */

/*------------------------------------------------------------------------
 *  strncpy  -  Copy string s2 to s1, truncating or null-padding
 *				to always copy n bytes.
 *				Return s1.
 *------------------------------------------------------------------------
 */
char		*strncpy(
			  char			*s1,		/* first string					*/
			  const char	*s2,		/* second string				*/
			  int			n			/* length of s2 to copy			*/
			)
{
    register int i;
    register char *os1;

    os1 = s1;
    for (i = 0; i < n; i++)
    {
        if (((*s1++) = (*s2++)) == '\0')
        {
            while (++i < n)
            {
                *s1++ = '\0';
            }
            return os1;
        }
    }
    return os1;
}
