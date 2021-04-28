/* atoi.c - atoi */

/*------------------------------------------------------------------------
 *  atoi  -  Converts an ascii value to an integer.
 *------------------------------------------------------------------------
 */
int	atoi(
	  char		*p
	)
{
    int n = 0, f = 0;

    for (;; p++)
    {
        switch (*p)
        {
        case ' ':
        case '\t':
            continue;
        case '-':
            f++;
        case '+':
            p++;
        }
        break;
    }

    while (*p >= '0' && *p <= '9')
    {
        n = n * 10 + *p++ - '0';
    }

    return (f ? -n : n);
}
