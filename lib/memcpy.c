/* memcpy.c - memcpy */

/*------------------------------------------------------------------------
 *  memcpy  -  Copy a location in memory from src to dst.
 *			   Return pointer to destination.
 *------------------------------------------------------------------------
 */
void		*memcpy(
			  void			*s,		/* destination location				*/
			  const void	*ct,	/* source location					*/
			  int			n		/* amount of data (in bytes) to copy*/
			)
{
    register int i;
    char *dst = (char *)s;
    char *src = (char *)ct;

    for (i = 0; i < n; i++)
    {
        *dst++ = *src++;
    }
    return s;
}
