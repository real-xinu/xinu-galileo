/* rsofile.c - rsofile */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * rsofile - open a file and place descriptor in ofiles table
 *------------------------------------------------------------------------
 */
int	rsofile (
	 char	*name,			/* file name			*/
	 int	mode			/* file mode to use		*/
	)
{
	int	i;			/* walks through ofiles table	*/
	char	*to;			/* ptr to name in ofiles	*/
	int	fd;			/* descriptor for new file	*/

	for (i=0; i<MAXFILES; i++) {
		if (ofiles[i].desc < 0) {
				break;
		}
	}

	if (i >= MAXFILES) {		/* if no space, return error */
		return -1;
	}

	/* space exists - open the specified file and check for error */

	fd = open(name, mode, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		return fd;
	}

	/* Place name and file descriptor in ofiles table */

	ofiles[i].desc = fd;
	to = ofiles[i].name;
	while (*to++ = *name++) {
		;
	}
	return 0;
}
