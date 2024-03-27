/* clock.h */

/* Intel 8254-2 clock chip constants */

#define	CLOCKBASE	0x40		/* I/O base port of clock chip	*/
#define	CLOCK0		CLOCKBASE
#define	CLKCNTL		(CLOCKBASE+3)	/* Chip CSW I/O port		*/


#define CLKTICKS_PER_SEC  1000		/* Clock timer resolution	*/

extern	uint32	clktime;		/* Second since system boot	*/
extern  uint32	count1000;		/* Ticks since clktime		*/

extern	qid16	sleepq;			/* Queue for sleeping processes	*/
extern	uint32	preempt;		/* Preemption counter		*/
