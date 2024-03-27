/* shell.c  -  shell */

#include <xinu.h>
#include <stdio.h>
#include "shprototypes.h"

/************************************************************************/
/* Table of Xinu shell commands and the function associated with each	*/
/************************************************************************/
const	struct	cmdent	cmdtab[] = {
	{"argecho",	xsh_argecho},
	{"arp",		xsh_arp},
	{"cat",		xsh_cat},
	{"clear",	xsh_clear},
	{"date",	xsh_date},
	{"devdump",	xsh_devdump},
	{"echo",	xsh_echo},
	{"help",	xsh_help},
	{"ls"	,	xsh_ls},
	{"kill",	xsh_kill},
	{"memdump",	xsh_memdump},
	{"memstat",	xsh_memstat},
	{"ns",		xsh_ns},
	{"netinfo",	xsh_netinfo},
	{"ping",	xsh_ping},
	{"ps",		xsh_ps},
	{"sleep",	xsh_sleep},
	{"tee",		xsh_tee},
	{"udp",		xsh_udpdump},
	{"udpecho",	xsh_udpecho},
	{"udpeserver",	xsh_udpeserver},
	{"uptime",	xsh_uptime},
	{"?",		xsh_help}

};

uint32	ncmd = sizeof(cmdtab) / sizeof(struct cmdent);


/************************************************************************/
/*									*/
/* cmdlookup -- return the index in cmdtab of a command name		*/
/*									*/
/************************************************************************/

int32	cmdlookup(char *name) {
	int32	indx;		/* Index into cmdtab			*/
	int32	len;		/* Length of command name		*/

	for (indx=0; indx < ncmd; indx++) {
		len = strlen(cmdtab[indx].cname);
		if ( strncmp(name,cmdtab[indx].cname, len) == 0 ) {
			return indx;
		}
	}
	return SYSERR;
}


/************************************************************************/
/* shell  -  Provide an interactive user interface that executes	*/
/*	     commands.  An input line contains one or more segments	*/
/*	     separated by a pipe symbol '|'.  Each segment begins with	*/
/*	     a command name, and has a set of optional arguments,  The	*/
/*	     last segment in a pipeline can have output redirected.	*/
/*	     The entire pipeline can be run in background by ending the	*/
/*	     line with an ampersand, "&".  The syntax is:		*/
/*									*/
/*		   seg [ pipe seg ]*  [ out_redir ]  [ & ]		*/
/*	     where:							*/
/*									*/
/*		pipe is		|					*/
/*									*/
/*		seg is		command_name [args*]			*/
/*									*/
/*		out_redir is	> output_file				*/
/*									*/
/************************************************************************/

process	shell (
		did32	dev		/* ID of tty device from which	*/
	)				/*   to accept commands		*/
{
	char	buf[SHELL_BUFLEN];	/* Input line (large enough for	*/
					/*   one line from a tty device	*/
	int32	len;			/* Length of line read		*/
	char	tokbuf[SHELL_BUFLEN +	/* Buffer to hold a set of	*/
			SHELL_MAXTOK];  /*   contiguous null-terminated	*/
					/*   strings of tokens		*/
	int32	tok[SHELL_MAXTOK];	/* Index of each token in	*/
					/*   array tokbuf		*/
	int32	toktyp[SHELL_MAXTOK];	/* Type of each token in tokbuf	*/
	int32	ntok;			/* Number of tokens on line	*/
	bool8	backgnd;		/* Run command in background?	*/
	char	*inname;		/* File name for input re-	*/
					/*   direction on first segment	*/
	did32	indesc;			/* Descriptor for redirected	*/
					/*   input			*/
	char	*outname;		/* File name for output re-	*/
					/*   direction on last segment	*/
	did32	outdesc;		/* Descriptor for redirected	*/
					/*   output			*/
	int32	nsegs;			/* Number of segments found	*/
	struct segent {			/* One segment of the pipeline	*/
	    int32	sstart;		/* Starting token index		*/
	    int32	send;		/* Ending token index		*/
	    int32	scindex;	/* Index in cmdtab of the	*/
					/*   command in this segment	*/
	    did32	soutdev;	/* Output device (pipe, except	*/
					/*   for first segment)		*/
	    did32	sindev;		/* Input device (pipe, except	*/
					/*   for the last segment)	*/
	    pid32	spid;		/* Process ID for this segment	*/
	};
	struct	segent	segtab[SHELL_MAXTOK];/* One entry per segment	*/
	struct	segent	*segptr;	/* Pointer to a segtab entry	*/
	int32	seg;			/* Index into segtab		*/
	int32	cindex;			/* Index of command returned	*/
					/*   by cmdlookup		*/
	did32	pipedev;		/* Device ID of a pipe device	*/
	int32	i;			/* Index into array of tokens	*/
	char	*p;			/* Pointer to cmd name		*/
	bool8	err;			/* Did an error occur?		*/
	int32	msg;			/* Message from receive() for	*/
					/*   child termination		*/
	int32	tmparg;			/* Temporary address used when	*/ 
					/*   creating a child process;	*/
					/*   later replaced by addargs	*/

	/* Print shell banner and startup message */

	fprintf(dev, "\n\n%s%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
		SHELL_BAN0,SHELL_BAN1,SHELL_BAN2,SHELL_BAN3,SHELL_BAN4,
		SHELL_BAN5,SHELL_BAN6,SHELL_BAN7,SHELL_BAN8,SHELL_BAN9);

	fprintf(dev, "%s\n\n", SHELL_STRTMSG);

	/* Continually prompt the user, read input, and execute command	*/

	while (TRUE) {

		/* Display prompt */

		fprintf(dev, SHELL_PROMPT);

		/* Read a command */

		len = read(dev, buf, sizeof(buf));

		/* Exit gracefully on end-of-file */

		if (len == EOF) {
			break;
		}

		/* If line only contains 'exit', exit gracefully */

		if ( (len==5) && (strncmp(buf,"exit\n", 5)==0) ) {
			break;
		}

		/* If line contains only NEWLINE, go to next line */

		if (len <= 1) {
			continue;
		}

		buf[len] = SH_NEWLINE;	/* Terminate line */

		/* Parse input line and divide into tokens */

		ntok = lexan(buf, len, tokbuf, tok, toktyp);

		/* Handle parsing error */

		if (ntok == SYSERR) {
			fprintf(dev,"%s\n", SHELL_SYNERRMSG);
			continue;
		}

		/* If line is empty, go to next input line */

		if (ntok == 0) {
			fprintf(dev, "\n");
			continue;
		}

		/* Set default input for first segment and output for 	*/
		/*   last segment to the device used to call the shell	*/

		/* See if the last token is '&', and set background */

		if (toktyp[ntok-1] != SH_TOK_AMPER) {
			backgnd = FALSE;
		} else {
			backgnd = TRUE;
			ntok-- ;
			if (ntok == 0 ) {
				fprintf(dev, "\n");
				continue;
			}
		}

		/* Use the pipe tokens to divide the pipeline into	*/
		/*  segments by placing the index of pipe tokens in	*/
		/*  successive locations of array segtab.		*/

		nsegs = 0;
		segptr = &segtab[nsegs];
		segptr->sstart = 0;
		for (i=0; i<ntok; i++) {

			/* Check for pipe at end of segment */

			if (toktyp[i] == SH_TOK_PIPE) {

				/* Finish old segment and start new */

				segptr->send = i - 1;
				
				/* Check for empty segment */
				
				if (segptr->sstart > segptr->send) {
				    fprintf(dev,"%s\n", SHELL_SYNERRMSG);
				    break;
				}
				/* Move to new segment */
				nsegs++;
				segptr = &segtab[nsegs];
				segptr->sstart = i+1;
			}
		}
		if (i < ntok) {
			/* Error occurred, so go to next input */
			continue;
		}

		/* Fill in remaining details for last segment */

		segptr->send = ntok - 1;
		if (segptr->sstart > segptr->send) {
			fprintf(dev,"%s\n", SHELL_SYNERRMSG);
			continue;
		}
		nsegs++;

		/* Check for output redirection on last segment */

		segptr = &segtab[nsegs - 1];
		outname = NULL;
		if ( ((segptr->send-segptr->sstart) > 1) &&
			(toktyp[segptr->send-1] == SH_TOK_GREATER) ) {
			if (toktyp[segptr->send] != SH_TOK_OTHER) {
				fprintf(dev,"%s\n", SHELL_SYNERRMSG);
				continue;
			}
			outname = &tokbuf[tok[segptr->send]];
			segptr->send -= 2;
		}


		/* Check for input redirection on first segment */

		segptr = &segtab[0];
		inname = NULL;
		if ( ((segptr->send-segptr->sstart) > 1) &&
			(toktyp[segptr->send-1] == SH_TOK_LESS) ) {
			if (toktyp[segptr->send] != SH_TOK_OTHER) {
				fprintf(dev,"%s\n", SHELL_SYNERRMSG);
				continue;
			}
			inname = &tokbuf[tok[segptr->send]];
			segptr->send -= 2;
		}


		/* Check that all tokens in each segment are "other" */

		err = FALSE;
		for (seg = 0; seg < nsegs; seg++) {
			segptr = &segtab[seg];
			for (i=segptr->sstart; i<= segptr->send; i++) {
			    if (toktyp[i] != SH_TOK_OTHER) {
				fprintf(dev,"%s\n", SHELL_SYNERRMSG);
				err = TRUE;
				break;
			    }
			}
			if (err) {
			    break;
			}
		}
		if (err) {
			continue;
		}

		/* For each command, look up the name of the command	*/

		for (seg = 0; seg< nsegs; seg++) {
		    segptr = &segtab[seg];
		    p = &tokbuf[tok[segptr->sstart]];
		    cindex = cmdlookup(p);
		    if (cindex == SYSERR) {
			fprintf(dev, "command %s not found\n", p); 
			break;
		    }
		    segptr->scindex = cindex;
		}
		if (seg < nsegs) {
			/* Error occurred, so go to next input */
			continue;
		}


		/* Open files for redirected input and output */

		indesc = outdesc = dev;

		if (inname != NULL) {
			indesc = open(NAMESPACE,inname,"ro");
			if (indesc == SYSERR) {
				fprintf(dev, SHELL_INERRMSG, inname);
				continue;
			}
		}
		segtab[0].sindev = indesc;

		if (outname != NULL) {
			outdesc = open(NAMESPACE,outname,"w");
			if (outdesc == SYSERR) {
				fprintf(dev, SHELL_OUTERRMSG, outname);

                		/* Close input file if it was opened */
                		if (indesc != dev) {
                			close(indesc);
        			}

				continue;
			}
			control(outdesc, F_CTL_TRUNC, 0, 0);
		}
		segtab[nsegs-1].soutdev = outdesc;

		/* Create nsegs-1 pipes (to go "between" the segments)	*/

		err = FALSE;
		for (seg = 0; seg < nsegs - 1; seg++) {
			segptr = &segtab[seg];
			pipedev = open(PIPE,NULLSTR,"rw");
			if (pipedev == SYSERR) {
				err = TRUE;
				fprintf(dev,"Pipe open failed\n");
				break;
			}
			segptr->soutdev = pipedev;
			segtab[seg+1].sindev= pipedev;
		}
		if (err) {
			/* Close previously opened pipes */
			for(seg--; seg >= 0; seg--) {
				close(segtab[seg].soutdev);
			}

            		/* Close previously opened infile */
            		if (indesc != dev) {
                		close(indesc);
            		}

            		/* Close previously opened outfile */
            		if (outdesc != dev) {
                		close(outdesc);
            		}

			continue;
		}

		/* Spawn a process for each segment  */


		int32 ntokens;

		err = FALSE;
		for (seg = 0; (seg<nsegs) && !err; seg++) {
		    segptr = &segtab[seg];
		    ntokens = segptr->send - segptr->sstart +1;
		    segptr->spid = create(cmdtab[segptr->scindex].cfunc,
			SHELL_CMDSTK, SHELL_CMDPRIO,
			cmdtab[segptr->scindex].cname, 2,
			ntokens, &tmparg);

		    /* If creation fails, report the error */

		    if (segptr->spid == SYSERR) {
			fprintf(dev, SHELL_CREATEMSG);
			err = TRUE;
			continue;
		    }

		    /* If adding arguments fails,report the error */

		    if (addargs(segptr->spid,ntokens,&tok[segptr->sstart],
				tokbuf, &tmparg)== SYSERR) {
			fprintf(dev, SHELL_CREATEMSG);
			err = TRUE;
			break;
		    }
		}
		if (err) {
		    /* Undo processes created before the error */
		    for(seg-- ; seg >= 0; seg--) {
			kill(segtab[seg].spid);
		    }
		}

		/* Redirect input and output for each process */
		for (seg=0; seg< nsegs; seg++) {
		    segptr = &segtab[seg];
		    proctab[segptr->spid].prdesc[0] = segptr->sindev;
		    proctab[segptr->spid].prdesc[1] = segptr->soutdev;
		}

		msg = recvclr();

		/* Resume each process in the pipeline (the shell will	*/
		/*   remain running because it has higher priority)	*/

		for (seg = 0; seg < nsegs; seg++) {
			resume(segtab[seg].spid);
		}

		/* Either block to wait for the last process in the	*/
		/*   pipeline to finish	or allow the pipeline to run	*/
		/*   in background					*/

		pid32	tmppid = segtab[nsegs-1].spid; /* Last seg. pid	*/
									
		if (! backgnd) {
			msg = receive();
			while (msg != tmppid) {
				msg = receive();
			}
		}
	}

    /* Terminate the shell process by returning from the top level */

    fprintf(dev,SHELL_EXITMSG);
    return OK;
}
