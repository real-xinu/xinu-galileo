/* timer.h - Decfinitions for the TCP timer */

#define NTIMERS		100		/* Number of timers		*/
#define BADTIMER	-1		/* Value to indicate invalid	*/

/* Parameters for the timer process */

#define TMSTK		8192		/* Size of process stack	*/
#define TMPRIO		600		/* Process proiority		*/
#define TMNAME		"timer"		/* Process name			*/

/* Timer entry */

struct	tmentry {
	int tm_remain;			/* Time remaining (in ms)	*/
	int tm_mq;			/* Message queue for the timer	*/
	int tm_msg;
	int tm_next;
};

extern	int32	tmpending;		/* Is a timer pending		*/
extern	int32	*tmnext;		/* Delay to next event		*/

extern	struct	tmentry tmtab[NTIMERS];	/* Table of timers		*/
extern	int32	tmfree;
extern	int32	tmhead;

process	timer ();
int32	tmfire ();

