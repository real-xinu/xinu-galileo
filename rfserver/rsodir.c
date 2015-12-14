/* rsodir.c - rsodir */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * rsodir - open a directory and place the pointer in the ofiles table
 *------------------------------------------------------------------------
 */
int	rsodir (
	char	*name	/* Directory name	*/
	)
{
	int	i;	/* walks through ofiles table	*/
	char	*to;	/* ptr to name in ofiles	*/
	DIR	*dirptr;/* ptr to opened directory	*/

	for(i = 0; i < MAXFILES; i++) {
		if((ofiles[i].desc == -1) && (ofiles[i].dirptr == NULL)) {
			break;
		}
	}

	if(i >= MAXFILES) {
		return -1;
	}

	dirptr = opendir(name);
	if(dirptr == NULL) {
		return -1;
	}

	ofiles[i].dirptr = dirptr;
	to = ofiles[i].name;
	while(*to++ = *name++) {
		;
	}

	return 0;
}
