/* strstr.c - strstr */

/*------------------------------------------------------------------------
 *  strstr  -  Returns a pointer to the location in a string at which
 *			   a particular string appears.
 *			   Return the pointer in the string, NULL if string not found.
 *------------------------------------------------------------------------
 */
char		*strstr(
			  const char		*cs,		/* string to search			*/
			  const char		*ct			/* string to locate			*/
			)
{
    char *cq;
    char *cr;

    for (; *cs != '\0'; cs++)
    {
        if (*cs == *ct)
        {
            cq = (char *)cs;
            cr = (char *)ct;
            while ((*cq != '\0') && (*cr != '\0'))
            {
                if (*cq != *cr)
                {
                    break;
                }
                cq++;
                cr++;
            }
            if ('\0' == *cr)
            {
                return (char *)cs;
            }
        }
    }
    return 0;
}
