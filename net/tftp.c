/* tftp.c - trivial file transfer protocol */

#include <xinu.h>
#include <stdlib.h>
#include <string.h>

/* Prototype for TFTP callback function */

typedef status (*tftp_recv_cb) (uint16, char*, uint32, byte);

/*------------------------------------------------------------------------
 *
 * tftp_send1  -  Internal function to send one outgoing request (RRQ or
 *		  ACK) message during a read sequence and get a matching
 *		  response (ignoring duplicates and nonsense packets)
 *
 *------------------------------------------------------------------------
 */

status	tftp_send1 (
	  int32		sock,		/* UDP socket to use		*/
	  uint32	remip,		/* Remote IP address		*/
	  uint16	*remport,	/* Remote port to use/set	*/
	  struct tftp_msg *msg,		/* Pointer to outgoing message	*/
	  int32		mlen,		/* Size of ougoing message	*/
	  struct tftp_msg *inmsg,	/* Pointer to buffer for an	*/
					/*   incoming message		*/
	  uint16	expected	/* Block number expected next	*/
	)
{
	int32	ret;			/* Return value	for udp_send	*/
	int32	n;			/* Number of bytes in response	*/
	uint32	tmp;			/* Holds IP address on receive	*/

	/*                 TFTP RRQ/WRQ Packet                  */
	/*   2 bytes     string    1 byte     string   1 byte   */
	/*   ------------------------------------------------   */
	/*  | Opcode |  Filename  |   0  |    Mode    |   0  |  */
	/*   ------------------------------------------------   */

	/*     TFTP ACK Packet       */
	/*   2 bytes     2 bytes     */
	/*   ---------------------   */
	/*  | Opcode |   Block #  |  */
	/*   ---------------------   */

	/* Send the outgoing message */

	ret = udp_sendto(sock, remip, *remport, (char *) msg, mlen);

	if (ret == SYSERR) {
		return SYSERR;
	}

	/* Repeatedly read incoming messages, discarding packets that	*/
	/*	are not valid or do not match the expected block number	*/
	/*	(i.e., duplicates of previous packets)			*/

	while(1) {

		/* Read next incoming message */

		n = udp_recvaddr(sock, &tmp, remport, (char *)inmsg,
				sizeof(struct tftp_msg), TFTP_WAIT);
		if (n == SYSERR) {
			return SYSERR;
		} else if (n == TIMEOUT) {
			kprintf("\n[tftp_send1] UDP Receive Timeout\n");
			return TIMEOUT;
		}

		if (n < 4) {	/* too small to be a valid packet */
			continue;
		}

		/* If Error came back, give up */

		if (ntohs(inmsg->tf_opcode) == TFTP_ERROR) {
			kprintf("\n[tftp_send1] TFTP Error %d, %s\n",
					ntohs(inmsg->tf_ercode),
					inmsg->tf_ermsg );
			return SYSERR;
		}

		/* If data packet matches expected block, return */

		if ( (ntohs(inmsg->tf_opcode) == TFTP_DATA) &&
				(ntohs(inmsg->tf_dblk) == expected)) {
			return n;
		}

		/* As this point, incoming message is either invalid or	*/
		/*	a duplicate, so ignore and try again		*/
	}
}

/*------------------------------------------------------------------------
 *
 * tftpget  -  Retrieve a file using TFTP
 *
 *------------------------------------------------------------------------
 */
status  tftpget(
	  uint32	serverip,	/* IP address of server		*/
	  const	char*	filename,	/* Name of the file to download	*/
	  void*		user_ptr,	/* User supplied buffer or	*/
					/*	function pointer	*/
	  uint32	user_len	/* Size of buffer or special	*/
					/*	value for function	*/
	)
{
	int32	nlen;			/* Length of file name		*/
	uint16	localport;		/* Local UDP port to use	*/
	uint16	remport=TFTP_PORT;	/* Remote UDP port to use	*/
	int32	sock;			/* Socket descriptor to use	*/
	uint16	expected = 1;		/* Next data block we expect	*/
	int32	i;			/* Loop index			*/
	int32	n;			/* Num,h. octets in incoming msg*/
	int32	ret;			/* Return value			*/
	int32	filesiz;		/* Total size of downloaded file*/
	struct	tftp_msg outmsg;	/* Outgoing message		*/
	int32	mlen;			/* Length of outgoing mesage	*/
	struct	tftp_msg inmsg;		/* Buffer for response message	*/
	int32	dlen;			/* Size of data in a response	*/
	char*	bptr;			/* Current user buff position	*/
	byte	lastpkt;		/* Indicator of last packet	*/

	/* Check args */

	if(filename == NULL || serverip == 0 || 
		user_ptr == NULL || user_len == 0) {
		kprintf("[TFTP GET] ERROR: Invalid argument\n");
		return SYSERR;
	}

	nlen = strnlen(filename, TFTP_MAXNAM+1);
	if ( (nlen <= 0) || (nlen > TFTP_MAXNAM) ) {
		return SYSERR;
	}

	/* Generate a local port */

	localport = getport();

	/* Register a UDP socket */

	sock = udp_register(serverip, 0, localport);
	if (sock == SYSERR) {
		kprintf("[TFTP Get] ERROR: udp_register failed\n");
		return SYSERR;
	}

	/* Clear the message buffer */

	memset((char*)&outmsg, NULLCH, sizeof(outmsg));

	/* Initialize the total file size to zero */

	filesiz = 0;
	bptr = (char*)user_ptr;
	lastpkt = FALSE;

	/* Form the first message and compute length (a Read Request)	*/

	outmsg.tf_opcode = htons(TFTP_RRQ);
	strncpy(outmsg.tf_filemode, filename, nlen+1);
	/* Set mode to 'octet' */
	strncpy(outmsg.tf_filemode+nlen+1, "octet", sizeof("octet")+1);

	/* Length is 2 opcode octets, name length, "octet" length,	*/
	/*		and two NULLs					*/

	mlen = nlen + strnlen("octet", 6) + 4;

	/* Repeatedly send the next request and get a response,		*/
	/*	retransmitting a request up to TFTP_MAXRETRIES times	*/

	while(1) {
		for (i=0; i < TFTP_MAXRETRIES; i++) {
			n = tftp_send1(sock, serverip, &remport, &outmsg, 
					mlen, &inmsg, expected);
			if (n > 0) {
				break;
			} else if (n == SYSERR) {
				kprintf("\n[TFTP Get] ERROR: TFTP Send "
					"fails\n");
				udp_release(sock);
				return SYSERR;
			} else if (n == TIMEOUT) {
				continue;

			}
		}
		if (i >= TFTP_MAXRETRIES) {
			kprintf("\n[TFTP Get] ERROR: Max retries %d "
				"exceeded\n", TFTP_MAXRETRIES);
			udp_release(sock);
			return SYSERR;
		}

		/* Compute size of data in the message */

		dlen = n - sizeof(inmsg.tf_opcode) - 
			sizeof(inmsg.tf_dblk);

		if(dlen < 512) {
			lastpkt = TRUE;
		}

		/* Move the contents of this block into the file buffer	*/

		for (i=0; i<dlen; i++) {
			if (user_len != TFTP_FUNC_MAGIC && 
				filesiz < user_len) {
				*bptr++ = inmsg.tf_data[i];
			}
			filesiz++;
		}

		/* Call the user supplied function */

		if(user_len == TFTP_FUNC_MAGIC) {
			((tftp_recv_cb)user_ptr)(ntohs(inmsg.tf_dblk), 
				inmsg.tf_data, dlen, lastpkt);

		}

		/* Form an ACK */

		outmsg.tf_opcode = htons(TFTP_ACK);
		outmsg.tf_ablk = htons(expected);
		mlen = sizeof(outmsg.tf_opcode) + sizeof(outmsg.tf_ablk);

		/* If this was the last packet, send final ACK */

		if (lastpkt) {
			ret = udp_sendto(sock, serverip, remport,
					(char *) &outmsg, mlen);
			udp_release(sock);

			if (ret == SYSERR) {
				kprintf("\n[TFTP GET] Error on final "
					"ack\n");
				return SYSERR;
			}

			return filesiz;
		}

		/* Move to next block and continue */

		expected++;
	}
}
