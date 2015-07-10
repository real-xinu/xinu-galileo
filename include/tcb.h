/* tcb.h - definitions used for TCP Control Blocks */

/* State of the TCB */

#define TCB_FREE	-1
#define TCB_CLOSED	0
#define TCB_LISTEN	1
#define TCB_SYNSENT	2
#define TCB_SYNRCVD	3
#define TCB_ESTD	4
#define TCB_FIN1	5
#define TCB_FIN2	6
#define TCB_CWAIT	7
#define TCB_CLOSING	8
#define TCB_LASTACK	9
#define TCB_TWAIT	10

/* Flags field in the TCP */

#define TCBF_FINSEEN	0x1
#define TCBF_RDDONE	0x2
#define TCBF_WRDONE	0x4
#define TCBF_NEEDACK	0x8
#define TCBF_RPUSHOK	0x10
#define TCBF_SPUSHOK	0x20
#define TCBF_ACKPEND	0x40
#define TCBF_RTTPEND	0x80

/* Form a command for a message queue or extract fields from a command */

#define TCBCMD(ptcb, cmd)	((((ptcb) - tcbtab) << 16) | (cmd))
#define TCBCMD_TCB(msg)		(&tcbtab[((msg) >> 16) & 0xffff])
#define TCBCMD_CMD(msg)		((msg) & 0xffff)

/* Commands that can be added to a message queue */

#define TCBC_SEND	1		/* Data is ready to send	*/
#define TCBC_DELACK	2		/* Schedule a delayed ACK	*/
#define TCBC_RTO	3		/* Round-trip Timer expires	*/
#define TCBC_EXPIRE	4		/* TCB expires			*/

#define TCPW_READERS	0x1
#define TCPW_WRITERS	0x2

/* Format of a TCP Control Block */

struct tcb {
	/* Device information */
	did32		tcb_dvnum;	/* Xinu device number		*/

	/* Allocated resources and precious state */
	int32		tcb_state;	/* Connection state in the FSM	*/
	int32		tcb_mutex;	/* Metadata exclusion		*/
	int32		tcb_rblock;	/* Reader block			*/
	int32		tcb_wblock;	/* Writer block			*/
	int32		tcb_lq;		/* Waiting connection queue	*/
	int32		tcb_ref;	/* Reference count		*/

	/* Connection identifying information */
	uint32		tcb_lip;	/* Local IP address		*/
	uint32		tcb_rip;	/* Remote host IP address	*/
	uint16		tcb_lport;	/* Local port number		*/
	uint16		tcb_rport;	/* Remote port number		*/

	/* Miscellaneous information */
	int32		tcb_flags;	/* Flags			*/
	int32		tcb_qlen;	/* # of tcbs in the listen queue*/
	int32		tcb_mss;	/* Maximum segment size		*/

	/* Receiver-side state */
	tcpseq		tcb_rnext;	/* RCV.NXT from RFC793		*/
	uint16		tcb_rwnd;	/* RCV.WND			*/
	tcpseq		tcb_rfin;	/* Receiver-side FIN seqno	*/
	tcpseq		tcb_rpush;	/* Received PSH seqno		*/
	int32		tcb_rbsize;	/* Receive buffer size		*/
	char *		tcb_rbdata;	/* First data byte in buffer	*/
	char *		tcb_rbend;	/* End of the buffer	*/
	tcpseq		tcb_rbseq;	/* Receive buffer 1st seqno	*/
	int32		tcb_rblen;	/* Length of data in buffer	*/
	char *		tcb_rbuf;	/* Receive buffer		*/
	int32		tcb_readers;	/* # of waiting readers		*/

	/* Sender-side state */
	tcpseq		tcb_suna;	/* SND.UNA from RFC793		*/
	tcpseq		tcb_snext;	/* SND.NXT			*/
	tcpseq		tcb_ssyn;	/* Outgoing SYN seqno		*/
	tcpseq		tcb_sfin;	/* Outgoing FIN seqno		*/
	tcpseq		tcb_spush;	/* Send PSH seqno		*/
	tcpseq		tcb_rttseq;	/* Seqno of RTT measurement	*/
	uint32		tcb_rtttime;	/* Time of RTT measurement	*/
	int32		tcb_cwnd;	/* JK88 cwnd			*/
	int32		tcb_ssthresh;	/* JK88 ssthresh		*/
	int32		tcb_dupacks;	/* Duplicate acks for suna	*/
	int32		tcb_srtt;	/* JK88 sa			*/
	int32		tcb_rttvar;	/* JK88 sv			*/
	int32		tcb_rto;	/* RTO in ms			*/
	int32		tcb_rtocount;	/* Successive RTOs		*/
	int32		tcb_sbsize;
	int32		tcb_sbdata;
	int32		tcb_sblen;
	char *		tcb_sbuf;
	int32		tcb_writers;
};

#define	Ntcp	5

extern struct tcb tcbtab[Ntcp];		/* the table of TCBs 		*/
