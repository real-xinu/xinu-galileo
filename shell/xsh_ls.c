/* xsh_ls.c - xsh_ls */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_ls - shell command to copy stdin to a file and to stdout
 *------------------------------------------------------------------------
 */
shellcmd xsh_ls(int nargs, char *args[])
{
	did32	dev;			/* Device to use		*/
	char	*fname;			/* File name to map to a device	*/
	struct	lfdir	dir;		/* Copy oif lfs directory	*/
	char	nbuf[NM_MAXLEN];	/* Buffer namespace can use for	*/
					/*   a replacement name		*/
	int32	retval;			/* Return value			*/
	bool8	loption = FALSE;	/* Did the user specify -l?	*/
	int32	i;			/* Walks through file names	*/
	int32	nextarg;		/* Walks through arguments	*/
	did32	fd;			/* File desc. for remote dir.	*/
	char	buf[512];		/* Buffer for remote directory	*/

	/* For argument '--help', emit help about the 'ls' command	*/

	if (nargs > 1 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [-l] [dir]\n\n", args[0]);
		printf("Description:\n");
		printf("\tlist files in the default directory or\n");
		printf("\tthe directory specfified by dir\n");
		printf("Options:\n");
		printf("\t-l\t list file size as well as name\n");
		printf("\t--help\t display this help and exit\n");
	}

	if (nargs > 3) {
		fprintf(stderr, "Use: %s [-l] [dir]\n\n", args[0]);
		return 1;
	}
	nextarg = 1;
	if (nargs > 1 && strncmp(args[nextarg], "-l", 3) == 0) {
		loption = TRUE;
		nextarg++;
		nargs--;
	}
	if (nargs > 1) {
		if (args[nextarg][0] == '-') {
			fprintf(stderr, "Use: %s [-l] [dir]\n\n",args[0]);
			return 1;
		}
		fname = args[nextarg];
	} else {
		fname = ".";
	}
	dev = namrepl(fname, nbuf);
	if (dev == (did32)SYSERR) {
		fprintf(stderr, "%s: cannot access device for %s\n",
			args[0], fname);
		return 1;
	}

	/* Handle local file system */

	if (dev == LFILESYS) {

	    wait(Lf_data.lf_mutex);
	    if ( ! Lf_data.lf_dirpresent ) {
		retval = read(Lf_data.lf_dskdev,(char *)&Lf_data.lf_dir,
			LF_AREA_DIR);
		if (retval == SYSERR ) {
			fprintf(stderr, "%s: cannot obtain directory\n",
				args[0]);
			signal(Lf_data.lf_mutex);
	                return 1;
		}
		if (lfscheck(&Lf_data.lf_dir) == SYSERR ) {
			fprintf(stderr, "%s: file system uninitialized\n",
				args[0]);
			signal(Lf_data.lf_mutex);
			return 1;
		}
	    }
	    signal(Lf_data.lf_mutex);
	    memcpy((void *)&dir, (void *)&Lf_data.lf_dir, sizeof(dir));
	    for (i = 0; i < dir.lfd_nfiles; i++) {
		if (loption) {
		    fprintf(stdout, "%6d  %s\n", dir.lfd_files[i].ld_size,
			dir.lfd_files[i].ld_name);
		} else {
			fprintf(stdout, "        %s\n",
			dir.lfd_files[i].ld_name);
		}
	    }

	/* Handle remote file system */

	} else if (dev == RFILESYS) {

	    fd = open(RFILESYS, nbuf, "r");
	    if (fd < 0){
		fprintf(stderr, "Cnanot open remote file %s\n", nbuf);
		return 1;
	    }
	    retval = read(fd, buf, 512);
	    while (retval > 0) {
		fprintf(stdout, "        %s\n",buf);
		retval = read(fd, buf, 512);
	    }
	    close(fd);
	    return 0;

	} else {

	    /* Handle erro case for unknown file system */

	    fprintf(stderr, "%s: file system is an unknown type\n",
		args[0]);
		return 1;
	}
	return 0;
}
