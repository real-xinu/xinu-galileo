/**
 * @file fgetc.c
 * @provides fgetc.
 *
 * $Id: fgetc.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/* fgetc.c - fgetc */

extern int getc(int);

#define EOF      (-2)

/*------------------------------------------------------------------------
 *  fgetc  -  Read a character from a device (file).
 *			  Return character read, EOF if error.
 *------------------------------------------------------------------------
 */
int		fgetc(
		  int		dev
		)
{
    int result = (int)getc(dev);

    if (0 > result)
    {
        return EOF;
    }
    else
    {
        return result;
    }
}
