/* strchr.c - strchr */

/*------------------------------------------------------------------------
 *  strchr  -  Returns a pointer to the location in a string at which
 *			   a particular character appears.
 *			   Return the pointer in the string, NULL if char not found.
 *------------------------------------------------------------------------
 */
char		*strchr(
			  const char		*s,		/* string to search				*/
			  int				c		/* character to locate			*/
			)
{
    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            return (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return 0;
}
