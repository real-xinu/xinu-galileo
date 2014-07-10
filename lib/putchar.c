/* putchar.c */

#include <xinu.h>
#include <stdio.h>

int putchar(int c)
{
    return fputc(c, stdout);
}
