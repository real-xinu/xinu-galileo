/* mq.h  -  constants and definitions for the TCP message queues	*/

#define	NMQ	70			/* Number of queues		*/

/* State of a queue */

#define MQ_FREE		0		/* Queue is unused		*/
#define MQ_ALLOC	1		/* Queue is in use		*/
#define MQ_DISABLE	2		/* Queue is in used but		*/
					/*   temporarily disabled	*/
#define MQ_CLEAR	3		/* Queue is being cleared	*/

/* Items associated with each queue */

struct	mqentry	{
	int32	mq_state;		/* State of the queue		*/
	int32	mq_first;		/*				*/
	int32	mq_count;		/* Count of entries		*/
	int32	mq_sem;			/* Semaphore for the queue	*/
	int32	mq_qlen;		/* Current length		*/
	int32	mq_rcvrs;		/*				*/
	int32	mq_cookie;		/* Cookie incremented when	*/
					/*   queue is created		*/
	int32	*mq_msgs;		/* Ptr to list of messages	*/
};

extern	struct	mqentry mqtab[NMQ];	/* The table of queues		*/
extern	int32	mqready;		/* Has mqinit been called?	*/

#define MQVALID(mq) ((mq) >= 0 && (mq) < NMQ)
