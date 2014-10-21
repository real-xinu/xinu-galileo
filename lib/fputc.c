/* fputc.c - fputc */

extern int putc(int, char);

#define	SYSERR   (-1)
#define EOF      (-2)

/*------------------------------------------------------------------------
 *  fputc  -  Write a character string to a device (file).
 *			  Return character written, EOF if error.
 *------------------------------------------------------------------------
 */
int		fputc(
		  int		c,				/* char to write					*/
		  int		dev				/* device to write to				*/
		)
{
    if (SYSERR == (int)putc(dev, c))
    {
        return EOF;
    }
    else
    {
        return c;
    }
}
