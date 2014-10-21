/* fscanf.c - fscanf, getch, ungetch */

#define EMPTY (-1)
#define EOF   (-2)

extern int getc(int);

static int getch(int, int);
static int ungetch(int, int);
extern int _doscan(register char *, register int **,
                   int (*getc) (int, int), int (*ungetc) (int, int), int,
                   int);

/*------------------------------------------------------------------------
 *  fscanf  -  Read from a device (file) according to a format.
 *			   Return result of _doscan.
 *------------------------------------------------------------------------
 */
int		fscanf(
		  int		dev,		/* device to read from					*/
		  char		*fmt,		/* format string						*/
		  int		args		/* number of arguments in format string */
		)
{
    int buf;

    buf = EMPTY;
    return (_doscan
            (fmt, (int **)&args, getch, ungetch, dev, (int)(int)&buf));
}

/*------------------------------------------------------------------------
 *  getch  -  Get a character from a device with pushback.
 *------------------------------------------------------------------------
 */
static int		getch(
				  int		dev,	/* device to read from				*/
				  int		abuf	/* buffer for reading into			*/
				)
{
    int *buf = (int *)abuf;

    if (*buf != EOF && *buf != EMPTY)
    {
        *buf = getc(dev);
    }
/* 	if( *buf != EOF ) */
/* 	{ *buf = control(dev, TTY_IOC_NEXTC, 0, 0); } */
    return (*buf);
}

/**
 * Pushback a character for getch.
 * @param dev device to push back to
 * @param abuf buffer for pushing back from
 */
/*------------------------------------------------------------------------
 *  ungetch  -  Pushback a character for getch.
 *------------------------------------------------------------------------
 */
static int		ungetch(
				  int		dev,		/* device to push back to		*/
				  int		abuf		/* buffer for pushing back from	*/
				)
{
    int *buf = (int *)abuf;

    *buf = EMPTY;
    return (*buf);
}
