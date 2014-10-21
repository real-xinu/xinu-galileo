/* memset.c - memset */

/*------------------------------------------------------------------------
 *  memset  -  Place a character into first n characters.
 *			   Return the pointer to memory.
 *------------------------------------------------------------------------
 */
void		*memset(
			  void		*s,			/* memory to place character into	*/
			  int		c,			/* character to place				*/
			  int		n			/* number of times to place char	*/
			)
{
    register int i;
    char *cp = (char *)s;

    for (i = 0; i < n; i++)
    {
        *cp = (unsigned char)c;
        cp++;
    }
    return s;
}
