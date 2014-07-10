/* ports.h - isbadport */

#define	NPORTS		30		/* maximum number of ports	*/
#define	PT_MSGS		100		/* total messages in system	*/
#define	PT_FREE		1		/* port is free			*/
#define	PT_LIMBO	2		/* port is being deleted/reset	*/
#define	PT_ALLOC	3		/* port is allocated		*/

struct	ptnode	{			/* node on list of messages 	*/
	uint32	ptmsg;			/* a one-word message		*/
	struct	ptnode	*ptnext;	/* ptr to next node on list	*/
};

struct	ptentry	{			/* entry in the port table	*/
	sid32	ptssem;			/* sender semaphore		*/
	sid32	ptrsem;			/* receiver semaphore		*/
	uint16	ptstate;		/* port state (FREE/LIMBO/ALLOC)*/
	uint16	ptmaxcnt;		/* max messages to be queued	*/
	int32	ptseq;			/* sequence changed at creation	*/
	struct	ptnode	*pthead;	/* list of message pointers	*/
	struct	ptnode	*pttail;	/* tail of message list		*/
};

extern	struct	ptnode	*ptfree;	/* list of free nodes		*/
extern	struct	ptentry	porttab[];	/* port table			*/
extern	int32	ptnextid;		/* next port ID to try when	*/
					/*   looking for a free slot	*/

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )
