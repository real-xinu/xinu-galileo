/* interrupt.h */

/* Definitions for the indirect interrupt dispatch array */

#define	MAX_EXT_IRQS	16	/* Total IRQs devoted to external	*/
				/* Actually, there are only 14 (32-38	*/
				/* and 40-46) because we send 39 to	*/
				/* spurious_irq7 and send 47 to	*/
				/* spurious_irq15.  However we declare	*/
				/* the action array so ti covers the	*/
				/* entire set.				*/

/* The following specifies the maximum number of devices that curently	*/
/*	share a given IRQ.						*/

#define	MAX_IRQ_SHARING	1	/* 1 means only one device is assigned	*/
				/* to a given IRQ (i.e., there is no	*/
				/* sharing).  If this value is changed,	*/
				/* INT_ENTRY_SIZE must also be changed	*/
				/* in include file interrupt.s		*/

#define INT_ENTRY_SIZE	16	/* This definition must be a constant	*/
				/* because it is used in assembly	*/
				/* language.  It *must* be equal to	*/
				/* sizeof(struct int_entry), which is	*/
				/*     8*(MAX_IRQ_SHARING + 1)		*/

#define	INT_NEXT_HAND	8	/* This definition must be a constant	*/
				/* because it is used in assembly	*/
				/* language.  It *must* be equal to	*/
				/*     sizeof(struct int_info)		*/

#ifndef	ASM

/* Definition of the information for one indirect interrupt reference	*/

struct int_info {
	void	(*int_handler) (int32); /* Address of a handler to call	*/
	int32	int_arg;	/* Argument to pass to the handler	*/
};

/* Definition of an array of indirect references for a given IRQ.  The	*/
/*	array has one entry for each device that shares the IRQ plus a	*/
/*	terminal entry of all zeros (i.e., to mark the end of the list).*/

struct int_entry {
	struct	int_info int_items[MAX_IRQ_SHARING+1];
	int32	nitems;
};


/* The interrupt actions array with one entry for each possible		*/
/*	external interrupt						*/


extern	struct	int_entry int_actions[MAX_EXT_IRQS];

/* Definition of the global interrupt mask */

extern	uint16	girmask;	/* mask of interrupts being serviced	*/
				/* a bit of 1 corresponds to an		*/
				/* interrupt that is allowed.		*/
#endif
