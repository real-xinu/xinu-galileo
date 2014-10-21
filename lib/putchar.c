/* putchar.c - putchar */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  putchar  -  DOCUMENT
 *------------------------------------------------------------------------
 */
int		putchar(
		  int		c
		)
{
    return fputc(c, stdout);
}
