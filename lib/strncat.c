/* strncat.c - strncat */

/*------------------------------------------------------------------------
 *  strncat  -  Concatenate s2 on the end of s1.
 *				S1's space must be large enough.
 *				At most n characters are moved.
 *------------------------------------------------------------------------
 */
char		*strncat(
			  char			*s1,		/* first string					*/
			  const char	*s2,		/* second string				*/
			  int			n			/* length to concatenate		*/
			)
{
    char *os1;

    os1 = s1;
    while (*s1++)
        ;
    --s1;
    while ((*s1++ = *s2++))
        if (--n < 0)
        {
            *--s1 = '\0';
            break;
        }
    return (os1);
}
