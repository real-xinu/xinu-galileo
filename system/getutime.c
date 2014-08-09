/* getutime.c - getutime */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getutime  -  Obtain time in seconds past Jan 1, 1970, UCT (GMT)
 *------------------------------------------------------------------------
 */
status	getutime(
	  uint32  *timvar		/* Location to store the result	*/
	)
{
	uint32	nnow;			/* Current time in network fmt	*/
	uint32	now;			/* Current time in xinu format	*/
	int32	retval;			/* Return value from call	*/
	uid32	slot;			/* Slot in UDP table		*/
	uint32	serverip;		/* IP address of a time server	*/
	char	prompt[2] = "xx";	/* Message to prompt time server*/

	if (Date.dt_bootvalid) {	/* Return time from local info	*/
		*timvar = Date.dt_boot + clktime;
		return OK;
	}

	/* Convert time server IP address to binary */

	if (dot2ip(TIMESERVER, &serverip) == SYSERR) {
		return SYSERR;
	}

	/* Contact the time server to get the date and time */

	slot = udp_register(serverip, TIMERPORT, TIMELPORT);
	if (slot == SYSERR) {
		fprintf(stderr,"getutime: cannot register a udp port %d\n",
					TIMERPORT);
		return SYSERR;
	}

	/* Send arbitrary message to prompt time server */

	if (getlocalip() == SYSERR) {
		return SYSERR;
	}	retval = udp_send(slot,	prompt, 2);
	if (retval == SYSERR) {
		fprintf(stderr,"getutime: cannot send a udp message %d\n",
					TIMERPORT);
		udp_release(slot);
		return SYSERR;
	}

	retval = udp_recv(slot, (char *) &nnow, 4, TIMETIMEOUT);
	if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
		udp_release(slot);
		return SYSERR;
	}
	udp_release(slot);
	now = ntim2xtim( ntohl(nnow) );
	Date.dt_boot = now - clktime;
	Date.dt_bootvalid = TRUE;
	*timvar = now;
	return OK;
}
