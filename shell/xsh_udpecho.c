/* xsh_udpecho.c - xsh_udpecho */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

extern	int	atoi(char *);

/*------------------------------------------------------------------------
 * xsh_udpecho - shell command that can send a message to a remote UDP
 *			echo server and receive a reply
 *------------------------------------------------------------------------
 */
shellcmd xsh_udpecho(int nargs, char *args[])
{
	int	i;			/* Index into buffer		*/
	int	retval;			/* Return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* Message to send	*/
	char	inbuf[1500];		/* Buffer for incoming reply	*/
	int32	slot;			/* UDP slot to use		*/
	int32	msglen;			/* Length of outgoing message	*/
	uint32	remoteip;		/* Remote IP address to use	*/
	uint16	remport= 7;		/* Remote port number to use	*/
	uint16	locport	= 52743;	/* Local port to use		*/
	int32	retries	= 3;		/* Number of retries		*/
	int32	delay	= 500;		/* Reception delay in ms	*/

	/* For argument '--help', emit help about the 'udpecho' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s  remote_ip [remote_port]\n\n", args[0]);
		printf("Description:\n");
		printf("\tBounce a message off a remote UDP echo server\n");
		printf("Options:\n");
		printf("\tremote_ip:\tIP address in dotted decimal\n");
		printf("\tremote_port:\tport number\n");
		printf("\t--help\t\tdisplay this help and exit\n");
		return 0;
	}

	/* Check for valid IP address argument */

	if ( (nargs < 2) || (nargs > 3) ) {
		fprintf(stderr, "%s: invalid number of argument(s)\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (dot2ip(args[1], &remoteip) == SYSERR) {
		fprintf(stderr, "%s: invalid IP address argument\n",
			args[0]);
		return 1;
	}
	if (nargs == 3) {
		retval = atoi(args[2]);
		if ( (retval <= 0) || (retval > 64535) ) {
			fprintf(stderr, "%s: invalid port argument\n",
				args[0]);
			return 1;
		}
		remport = (uint16) retval;
	}

	fprintf(stderr, "using remote port %d\n", remport);

	/* Register local UDP port */

	slot = udp_register(remoteip, remport, locport);
	if (slot == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				args[0], locport);
		return 1;
	}

	/* Retry sending outgoing datagram and getting response */

	msglen = strnlen(msg, 1200);
	for (i=0; i<retries; i++) {
		retval = udp_send(slot, msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				args[0]);
			return 1;
		}

		retval = udp_recv(slot, inbuf, sizeof(inbuf), delay);
		if (retval == TIMEOUT) {
			fprintf(stderr, "%s: timeout...\n", args[0]);
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error from udp_recv \n",
				args[0]);
			udp_release(slot);
			return 1;
		}
		break;
	}

	udp_release(slot);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: retry limit exceeded\n",
			args[0]);
		return 1;
	}

	/* Response received - check contents */

	if (retval != msglen) {
		fprintf(stderr, "%s: sent %d bytes and received %d\n",
			args[0], msglen, retval);
		return 1;
	}
	for (i = 0; i < msglen; i++) {
		if (msg[i] != inbuf[i]) {
			fprintf(stderr, "%s: reply differs at byte %d\n",
				args[0], i);
			return 1;
		}
	}

	printf("UDP echo test was successful\n");
	return 0;
}
