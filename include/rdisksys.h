/* rdisksys.h - definitions for remote disk system pseudo-devices */

#ifndef	Nrds
#define	Nrds		1
#endif

/* Remote disk block size */

#define	RD_BLKSIZ	512

/* Global data for the remote disk server */

#ifndef	RD_SERVER
#define	RD_SERVER	"example.com"
#endif

#ifndef	RD_SERVER_PORT
#define	RD_SERVER_PORT	53124
#endif

#ifndef	RD_LOC_PORT
#define	RD_LOC_PORT	53124		/* Base port number - minor dev	*/
					/*   number is added to insure	*/
					/*   that each device is unique	*/
#endif

#define	RD_SSIZE	NPROC		/* Number of serial queue nodes	*/
#define	RD_QNODES	30		/* Number of request queue nodes*/
#define	RD_CNODES	32		/* Number of cache bodes	*/


/* Constants for remote disk device control block */

#define	RD_IDLEN	64		/* Size of a remote disk ID	*/
#define	RD_STACK	16384		/* Stack size for comm. process	*/
#define	RD_PRIO		600		/* Priorty of comm. process	*/
					/*  (Must be higher than any	*/
					/*   process that reads/writes	*/

/* Constants for state of the device */

#define	RD_CLOSED	0		/* Device is not in use		*/
#define	RD_OPEN		1		/* Device is open		*/
#define	RD_PEND		2		/* Device is being opened	*/
#define RD_DELETING	3		/* Device is being deleted	*/

/* Operations for request queue */

#define	RD_OP_READ	1		/* Read operation on req. list	*/
#define	RD_OP_WRITE	2		/* Write operation on req. list	*/
#define	RD_OP_SYNC	3		/* Sync operation on req. list	*/

/* Definition of a serial queue node */

struct	rdsent {			/* Entry in the serial queue	*/
	int32	rd_op;			/* Operation - read/write/sync	*/
	uint32	rd_blknum;		/* Disk block number to use	*/
	char	*rd_callbuf;		/* Address of caller's buffer	*/
	pid32	rd_pid;			/* Process that initiated the	*/
};					/*   request			*/

/* Definition of a request queue node */

struct	rdqnode {			/* Node in the request queue	*/
	struct	rdqnode	*rd_next;	/* Pointer to next node		*/
	struct	rdqnode	*rd_prev;	/* Pointer to previous node	*/
	int32	rd_op;			/* Operation - read/write/sync	*/
	uint32	rd_blknum;		/* Disk block number to use	*/
	char	*rd_callbuf;		/* Address of caller's buffer	*/
	pid32	rd_pid;			/* Process making the request	*/
	char	rd_wbuf[RD_BLKSIZ];	/* Data for a write operation	*/
};

/* Definition of a node in the cache */

struct	rdcnode {			/* Node in the cache		*/
	struct	rdcnode	*rd_next;	/* Pointer to next node		*/
	struct	rdcnode	*rd_prev;	/* Pointer to previous node	*/
	uint32	rd_blknum;		/* Number of this disk block	*/
	byte	rd_data[RD_BLKSIZ];	/* Data for the disk block	*/
};

/* Device control block for a remote disk */

struct	rdscblk	{			/* Remote disk control block	*/
	int32	rd_state;		/* State of device		*/
	char	rd_id[RD_IDLEN];	/* Disk ID currently being used	*/
	int32	rd_seq;			/* Next sequence number to use	*/
	struct	rdcnode	*rd_chead;	/* Head of cache		*/
	struct	rdcnode	*rd_ctail;	/* Tail of cache		*/
	struct	rdcnode	*rd_cfree;	/* Free list of cache nodes	*/
	struct	rdqnode	*rd_qhead;	/* Head of request queue	*/
	struct	rdqnode	*rd_qtail;	/* Tail of request queue	*/
	struct	rdqnode	*rd_qfree;	/* Free list of request nodes	*/
	struct	rdsent	rd_sq[RD_SSIZE];/* Serial queue circular buffer	*/
	int32	rdshead;		/* Head of the serial queue	*/
	int32	rdstail;		/* Tail of the serial queue	*/
	int32	rdscount;		/* Count serial queue items	*/
	pid32	rd_comproc;		/* Process ID of comm. process	*/
	uint32	rd_ser_ip;		/* Server IP address		*/
	uint16	rd_ser_port;		/* Server UDP port		*/
	uint16	rd_loc_port;		/* Local (client) UPD port	*/
	bool8	rd_registered;		/* Has UDP port been registered?*/
	int32	rd_udpslot;		/* Registered UDP slot		*/
};

extern	struct	rdscblk	rdstab[];	/* Remote disk control block	*/

/* Definitions of parameters used during server access */

#define	RD_RETRIES	3		/* Times to retry sending a msg	*/
#define	RD_TIMEOUT	1000		/* Timeout for reply (1 second)	*/

/* Control functions for a remote disk device */

#define RDS_CTL_SYNC	1		/* Write all pending blocks	*/
#define	RDS_CTL_DEL	2		/* Delete the entire disk	*/
#define	RDS_CTL_CLOSE	3		/* Close the disk device	*/

/************************************************************************/
/*	Definition of messages exchanged with the remote disk server	*/
/************************************************************************/
/* Values for the type field in messages */

#define	RD_MSG_RESPONSE	0x0100		/* Bit that indicates response	*/

#define	RD_MSG_RREQ	0x0010		/* Read request and response	*/
#define	RD_MSG_RRES	(RD_MSG_RREQ | RD_MSG_RESPONSE)

#define	RD_MSG_WREQ	0x0020		/* Write request and response	*/
#define	RD_MSG_WRES	(RD_MSG_WREQ | RD_MSG_RESPONSE)

#define	RD_MSG_OREQ	0x0030		/* Open request and response 	*/
#define	RD_MSG_ORES	(RD_MSG_OREQ | RD_MSG_RESPONSE)

#define	RD_MSG_CREQ	0x0040		/* Close request and response	*/
#define	RD_MSG_CRES	(RD_MSG_CREQ | RD_MSG_RESPONSE)

#define	RD_MSG_DREQ	0x0050		/* Delete request and response 	*/
#define	RD_MSG_DRES	(RD_MSG_DREQ | RD_MSG_RESPONSE)

#define	RD_MIN_REQ	RD_MSG_RREQ	/* Minimum request type		*/
#define	RD_MAX_REQ	RD_MSG_DREQ	/* Maximum request type		*/

/* Message header fields present in each message */

#define	RD_MSG_HDR			/* Common message fields	*/\
	uint16	rd_type;		/* Message type			*/\
	uint16	rd_status;		/* 0 in req, status in response	*/\
	uint32	rd_seq;			/* Message sequence number	*/\
	char	rd_id[RD_IDLEN];	/* Null-terminated disk ID	*/

/************************************************************************/
/*				Header					*/
/************************************************************************/
/* The standard header present in all messages with no extra fields */
#pragma pack(2)
struct	rd_msg_hdr {			/* Header fields present in each*/
	RD_MSG_HDR			/*   remote disk system message	*/
};
#pragma pack()

/************************************************************************/
/*				Read					*/
/************************************************************************/
#pragma pack(2)
struct	rd_msg_rreq	{		/* Remote disk read request	*/
	RD_MSG_HDR			/* Header fields		*/
	uint32	rd_blk;			/* Block number to read		*/
};
#pragma pack()

#pragma pack(2)
struct	rd_msg_rres	{		/* Remote disk read reply	*/
	RD_MSG_HDR			/* Header fields		*/
	uint32	rd_blk;			/* Block number that was read	*/
	char	rd_data[RD_BLKSIZ];	/* Array containing one block	*/
};
#pragma pack()

/************************************************************************/
/*				Write					*/
/************************************************************************/
#pragma pack(2)
struct	rd_msg_wreq	{		/* Remote disk write request	*/
	RD_MSG_HDR			/* Header fields		*/
	uint32	rd_blk;			/* Block number to write	*/
	char	rd_data[RD_BLKSIZ];	/* Array containing one block	*/
};
#pragma pack()

#pragma pack(2)
struct	rd_msg_wres	{		/* Remote disk write response	*/
	RD_MSG_HDR			/* Header fields		*/
	uint32	rd_blk;			/* Block number that was written*/
};
#pragma pack()

/************************************************************************/
/*				Open					*/
/************************************************************************/
#pragma pack(2)
struct	rd_msg_oreq	{		/* Remote disk open request	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()

#pragma pack(2)
struct	rd_msg_ores	{		/* Remote disk open response	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()

/************************************************************************/
/*				Close					*/
/************************************************************************/
#pragma pack(2)
struct	rd_msg_creq	{		/* Remote disk close request	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()

#pragma pack(2)
struct	rd_msg_cres	{		/* Remote disk close response	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()

/************************************************************************/
/*				Delete					*/
/************************************************************************/
#pragma pack(2)
struct	rd_msg_dreq	{		/* Remote disk delete request	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()

#pragma pack(2)
struct	rd_msg_dres	{		/* Remote disk delete response	*/
	RD_MSG_HDR			/* Header fields		*/
};
#pragma pack()
