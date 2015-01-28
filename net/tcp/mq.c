/* mq.c  -  mqcreate, mqdelete, mqsend, mqrecv, mqpoll, mqdisable,	*/
/*			mqenable, mqclear				*/

#include <xinu.h>

struct	mqentry	mqtab[NMQ];		/* Table of message queues	*/
sid32	mqsem;				/* Mutex semaphore		*/
int32	mqready = 0;

/* Semaphore semantics:							*/
/* 	mqcreate and mqdelete hold the mutex to avoid disabling		*/
/*	interrupts during the iteration.  However, interrupts must 	*/
/*	be disabled when an item's MQ_FREE/MQ_ALLOC status is changed.	*/
/*	Therefore, disabling interrupts is used to send or receive	*/
/*	a message.  The point is: mqsem must be held to add or remove	*/
/*	a queue, and interrupts must be disabled for the actual		*/
/*	change in status;  during send or receive, interrupts must be	*/
/*	disabled for all modifications.					*/

/*------------------------------------------------------------------------
 *  mqinit  -  Initialize the message queues that will be used by TCP
 *------------------------------------------------------------------------
 */
void	mqinit(void)
{
	int32	i;			/* index into queue table	*/

	/* Set the state to free and initialize the cookie */

	for (i=0; i<NMQ; i++) {
		mqtab[i].mq_state = MQ_FREE;
		mqtab[i].mq_cookie = 0;
	}

	/* Create the mutex */

	mqsem = semcreate(1);
	mqready = 1;
}


/*------------------------------------------------------------------------
 *  mqcreate  -  create a message queue and return its ID
 *------------------------------------------------------------------------
 */
int32	mqcreate (
	  int32		qlen		/* Length of the queue		*/
	)
{
	int32	i;			/* Index into queue table	*/
	intmask	mask;			/* Saved interrupt mask		*/

	/* Guarantee mutual exclusion */

	wait(mqsem);

	/* Find a free entry in the table */

	for (i = 0; i < NMQ; i++) {
		if (mqtab[i].mq_state != MQ_FREE)
			continue;

		/* Create a semaphore to control access */

		if ((mqtab[i].mq_sem = semcreate (0)) == SYSERR) {
			kprintf("error1 i = %d\n", i);
			signal (mqsem);
			return SYSERR;
		}

		/* Allocate memory to hold a set of qlen messages */

		if ((mqtab[i].mq_msgs = (int32 *)getmem (qlen * sizeof(int32)))
		    == (int32 *)SYSERR) {
		    	kprintf("error2\n");
			semdelete(mqtab[i].mq_sem);
			signal (mqsem);
			return SYSERR;
		}

		/* Initialize remaining entries for the queue structure */


		mqtab[i].mq_qlen = qlen;
		mqtab[i].mq_first = 0;
		mqtab[i].mq_count = 0;
		mqtab[i].mq_rcvrs = 0;
		mqtab[i].mq_cookie++;
		mask = disable();
		mqtab[i].mq_state = MQ_ALLOC;
		restore(mask);
		signal (mqsem);
		return i;
	}

	/* Table is full */
	kprintf("error3\n");
	signal (mqsem);
	return SYSERR;
}

/*------------------------------------------------------------------------
 *  mqdelete  -  delete a message queue
 *------------------------------------------------------------------------
 */
int32	mqdelete (
	  int32		mq		/* Message queue to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	mqentry	*mqp;		/* ptr to queue entry		*/

	/* Obtain a pointer to the specified queue */

	mqp = &mqtab[mq];

	/* Guarantee exclusive access and check status */

	wait (mqsem);
	if (!MQVALID(mq) || mqp->mq_state != MQ_ALLOC) {
		signal (mqsem);
		return SYSERR;
	}

	/* Make the queue free and return the memory */

	mask = disable();
	mqp->mq_state = MQ_FREE;
	restore(mask);
	semdelete (mqp->mq_sem);
	freemem ((char *)mqp->mq_msgs, mqp->mq_qlen * sizeof(int));
	signal (mqsem);
	return OK;
}

/*------------------------------------------------------------------------
 *  mqsend  -  deposit a message on a queue
 *------------------------------------------------------------------------
 */
int32	mqsend (
	  int32		mq,		/* Message queue to use		*/
	  int32		msg		/* Message to deposit		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	mqentry	*mqp;		/* ptr to message queue		*/

	/* Obtain ptr to specified queue */

	mqp = &mqtab[mq];

	/* Insure exclusive access */

	mask = disable();
	if (!MQVALID(mq) || mqp->mq_state != MQ_ALLOC
	    || mqp->mq_count == mqp->mq_qlen) {
		restore(mask);
		return SYSERR;
	}

	/* Add message in next available slot and increase count */

	mqp->mq_msgs[(mqp->mq_first + mqp->mq_count) % mqp->mq_qlen] = msg;
	mqp->mq_count++;

	/* Make message available */

	signal (mqp->mq_sem);

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *  mqrecv  -  extract a message from a queue
 *------------------------------------------------------------------------
 */
int32	mqrecv (
	  int32		mq		/* Message queue to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	mqentry	*mqp;		/* Ptr to queue			*/
	int32	msg;			/* Extracted message		*/
	int32	cookie;			/* Old value of cookie		*/

	/* Obtain a pointer ot the queue */

	mqp = &mqtab[mq];

	/* Insure exclusive access */

	mask = disable();
	if (!MQVALID(mq) || mqp->mq_state != MQ_ALLOC) {
		restore(mask);
		return SYSERR;
	}

	/* Record current cookie */

	cookie = mqp->mq_cookie;
	mqp->mq_rcvrs++;
	wait(mqp->mq_sem);
	if (cookie != mqp->mq_cookie) {	  /* queue changed */
		restore(mask);
		return SYSERR;
	}
	mqp->mq_rcvrs--;

	/* Extract message and move 'first' pointer to next message */

	msg = mqp->mq_msgs[mqp->mq_first];
	mqp->mq_first = (mqp->mq_first + 1) % mqp->mq_qlen;

	/* Decrement count of messages remaining and return msg */

	mqp->mq_count--;
	restore(mask);
	return msg;
}

/*------------------------------------------------------------------------
 *  mqpoll  -  Extract a message from a queue if one is available
 *------------------------------------------------------------------------
 */
int32	mqpoll (
	  int32		mq		/* Message queue to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	msg;			/* Message to return		*/

	/* Insure exclusive access */

	mask = disable();
	if (!MQVALID(mq) || mqtab[mq].mq_state != MQ_ALLOC
	    || mqtab[mq].mq_count == 0) {	/* No message available	*/
		restore(mask);
		return SYSERR;
	}

	/* Extract a message and return */

	msg = mqrecv (mq);
	restore(mask);
	return msg;
}

/*------------------------------------------------------------------------
 *  mqdisable  -  Temporarily disable a message queue
 *------------------------------------------------------------------------
 */
int32	mqdisbale (
	  int32		mq		/* Message queue to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	mqentry	*mqp;		/* Ptr to the queue		*/

	/* Obtain pointer to the queue */

	mqp = &mqtab[mq];

	/* Insure exclusive access */

	wait (mqsem);

	/* Disable interrupts for state change */

	mask = disable();
	if (!MQVALID(mq) || mqp->mq_state != MQ_ALLOC) {
		restore(mask);
		signal (mqsem);
		return SYSERR;
	}
	mqp->mq_cookie++;
	mqp->mq_state = MQ_DISABLE;

	/* Reset semaphore in case processes are waiting */

	semreset(mqp->mq_sem, 0);
	mqp->mq_rcvrs = 0;

	restore(mask);
	signal (mqsem);
	return OK;
}

/*------------------------------------------------------------------------
 *  mqenable  -  Re-enable a message queue that has been disabled
 *------------------------------------------------------------------------
 */
int32	mqenable (
	  int32		mq		/* Message queue to use		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	retval;			/* Return value			*/

	/* Insure exclusive access */

	wait (mqsem);
	mask = disable();

	/* Re-enable if currently disabled */
	if (MQVALID(mq) && mqtab[mq].mq_state == MQ_DISABLE) {
		mqtab[mq].mq_state = MQ_ALLOC;
		retval = OK;
	} else {
		retval = SYSERR;
	}
	restore(mask);
	signal (mqsem);
	return retval;
}

/*------------------------------------------------------------------------
 *  mqclear  -  clear all messages from a queue
 *------------------------------------------------------------------------
 */
int32	mqclear (
	  int32		mq,		/* Message queue to use		*/
	  int32		(*func)(int32)	/* Message disposal function	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	mqentry	*mqp;		/* Ptr to the queue		*/
	int32	i;			/* walks through message list	*/

	/* Obtain a pointer to the queue */

	mqp = &mqtab[mq];

	/* Obtain exclusive access and insure queue is disabled */

	wait (mqsem);
	mask = disable();
	if (!MQVALID(mq) || mqp->mq_state != MQ_DISABLE) {
		restore(mask);
		signal (mqsem);
		return SYSERR;

	}

	/* Set the state to "clearing" during iteration */

	mqp->mq_state = MQ_CLEAR;
	restore(mask);
	signal (mqsem);

	/* Iterate through all messages and use func to dispose of each	*/

	for (i = 0; i < mqp->mq_count; i++) {
		(*func)(mqp->mq_msgs[(mqp->mq_first + i) % mqp->mq_qlen]);
	}
	mqp->mq_count = 0;

	mask = disable();

	/* Reset the state to disabled*/

	mqp->mq_state = MQ_DISABLE;
	restore(mask);

	return OK;
}
