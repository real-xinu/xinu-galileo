/* timer.c  -  tminit, timer, tmfire, tmset, tmdel */

#include <xinu.h>

int32	tmpending;
int32	*tmnext;

struct	tmentry	tmtab[NTIMERS];
int32	tmfree;
int32	tmhead;
int32	tmsem;
int32	tmlock;

/*------------------------------------------------------------------------
 *  tminit  -  Initialize the TCP timers
 *------------------------------------------------------------------------
 */
void	tminit(void)
{
	int32	i;			/* iterates through timers	*/

	/* Initialize globals */

	tmpending = 0;
	tmnext = NULL;
	tmfree = 0;

	/* Link table entries together except for last one */

	for (i = 0; i < NTIMERS - 1; i++) {
		tmtab[i].tm_next = i + 1;
	}
	tmtab[i].tm_next = BADTIMER;

	/* List is initially empty */

	tmhead = BADTIMER;

	/* Create consumer and mutex semaphores */

	tmsem = semcreate (0);
	tmlock = semcreate (1);

	/* Create the timer process */

	resume (create ((int *)timer, TMSTK, TMPRIO, TMNAME, 0, 0));
}

/*------------------------------------------------------------------------
 *  timer  -  The TCP timer process
 *------------------------------------------------------------------------
 */
process	timer(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	tmp;			/* Head pointer during deletion	*/

	while (1) {

		/* Wait for work */

		wait (tmsem);

		/* Obtain exclusive access */
		wait (tmlock);

		/* When time expires send a message */

		while (tmhead != BADTIMER && tmtab[tmhead].tm_remain <= 0) {
			mqsend (tmtab[tmhead].tm_mq, tmtab[tmhead].tm_msg);
			tmp = tmhead;
			tmhead = tmtab[tmhead].tm_next;
			tmtab[tmp].tm_next = tmfree;
			tmfree = tmp;
		}

		/* If timed events remain, update pending and next */

		if (tmhead != BADTIMER) {
			mask = disable();
			tmpending = TRUE;
			tmnext = &tmtab[tmhead].tm_remain;
			restore(mask);
		}
		signal (tmlock);
	}
}

/*------------------------------------------------------------------------
 *  tmfire  -  Function called by clock interrupt when time has expired
 *			for first pending event
 *------------------------------------------------------------------------
 */
int32	tmfire ()
{
	tmpending = FALSE;
	signal (tmsem);
	return 0;
}

/*------------------------------------------------------------------------
 *  tmset  -  Set a timed event
 *------------------------------------------------------------------------
 */
int32	tmset (
	  int32	delay,			/* Delay in milliseconds	*/
	  int32	mq,			/* Message queue for the event	*/
	  int32	msg			/* Message to be sent		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	tment;			/* Entry from free list		*/
	int32	cur;			/* Current list entry		*/
	int32	prev;			/* Previous list entry		*/

	/* Obtain exclusive access */

	wait (tmlock);
	if (tmfree == BADTIMER) {

		/* No nodes left on free list */

		signal (tmlock);
		return SYSERR;
	}

	/* Obtain first node from free list and unlink */
	tment = tmfree;
	tmfree = tmtab[tmfree].tm_next;

	/* Fill in values in the node */

	tmtab[tment].tm_mq = mq;
	tmtab[tment].tm_msg = msg;
	tmtab[tment].tm_next = BADTIMER;

	/* Walk the event delta list and insert the new item */
	prev = BADTIMER;
	cur = tmhead;
	mask = disable();
	while (cur != BADTIMER) {

		/* Compare delay for new item to item in list */

		if (tmtab[cur].tm_remain > delay) {
			tmtab[cur].tm_remain -= delay;
			break;
		}

		/* Invariant: delay is relative to current list item */

		delay -= tmtab[cur].tm_remain;
		prev = cur;
		cur = tmtab[cur].tm_next;
	}
	if (prev == BADTIMER) {

		/* New item is at beginning of list */

		tmhead = tment;
	} else {

		/* New item is not at beginning of list */

		tmtab[prev].tm_next = tment;
	}
	tmtab[tment].tm_next = cur;
	tmtab[tment].tm_remain = delay;

	tmpending = TRUE;
	tmnext = &tmtab[tmhead].tm_remain;
	restore(mask);
	signal (tmlock);
	return OK;
}


/*------------------------------------------------------------------------
 *  tmdel  -  Delete a timed event
 *------------------------------------------------------------------------
 */
int32	tmdel (
	  int32	mq,			/* Message queue for the event	*/
	  int32	msg			/* Message to be sent		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32	cur;			/* Current list entry		*/
	int32	prev;			/* Previous list entry		*/
	int32	next;			/* Next list entry		*/
	int32	found;			/* Was item found?		*/

	/* Obtain exclusive access */

	wait (tmlock);

	/* Prepare to walk the list */

	prev = BADTIMER;
	cur = tmhead;
	found = 0;

	/* Walk the list and search for specified item */

	while (cur != BADTIMER) {
		if (tmtab[cur].tm_mq == mq && tmtab[cur].tm_msg == msg) {
			if (prev == BADTIMER) {

				/* Delete first item on list */

				tmhead = tmtab[cur].tm_next;
			} else {

				/* Delete item that is not first */

				tmtab[prev].tm_next = tmtab[cur].tm_next;
			}

			/* If an item follows, adjust the time in the	*/
			/*	item to account for deleted item	*/

			if ((next = tmtab[cur].tm_next) != BADTIMER) {
				mask = disable();
				tmtab[next].tm_remain += 
						tmtab[cur].tm_remain;
				restore(mask);
			}

			/* Put the node back on the free list */

			tmtab[cur].tm_next = tmfree;
			tmfree = cur;
			found = 1;
			break;
		}
		prev = cur;
		cur = tmtab[cur].tm_next;
	}

	/* If node not found, report to caller */

	if (!found) {
		signal (tmlock);
		return SYSERR;
	}

	/* If items remain on list, reset info for clock interrupt */

	mask = disable();
	if (tmhead == BADTIMER) {

		/* No items remain on list */

		tmpending = FALSE;
	} else {

		/* At least one item remains on list */

		tmpending = TRUE;
		tmnext = &tmtab[tmhead].tm_remain;
	}
	restore(mask);
	signal (tmlock);
	return OK;
}
