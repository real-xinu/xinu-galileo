/* getchar.c */

#include <xinu.h>
#include <stdio.h>

int getchar(void)
{
    return fgetc(stdin);
}
