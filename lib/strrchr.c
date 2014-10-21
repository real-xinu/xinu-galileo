/* strrchr.c - strrchr */

/*------------------------------------------------------------------------
 *  strrchr  -  Returns a pointer to the location in a string at which
 *				a particular character last appears.
 *				Return the pointer in the string, NULL if char not found.
 *------------------------------------------------------------------------
 */
char		*strrchr(
			  const char		*s,			/* string to search			*/
			  int				c			/* character to locate		*/
			)
{
    char *r = 0;

    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            r = (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return r;
}
