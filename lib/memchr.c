/* memchr.c - memchr */

/*------------------------------------------------------------------------
 *  memchr  -  Return a pointer to the lecation in memory at which
 *			   a particular character appears.
 *			   Return the pointer in the string, NULL if char not found.
 *------------------------------------------------------------------------
 */
void		*memchr(
			  const void	*cs,		/* string to search				*/
			  int			c,			/* character to locate			*/
			  int			n			/* number of bytes to search	*/
			)
{
    char *cp = (char *)cs;

    for (; *cp != '\0'; cp++)
    {
        if (*cp == (unsigned char)c)
        {
            return (void *)cp;
        }
    }
    return 0;
}
