/* evec.c - initevec, doevec */

#include <xinu.h>

/* Interrupt Descriptor */

struct __attribute__ ((__packed__)) idt {
	unsigned short	igd_loffset;
	unsigned short	igd_segsel;
	unsigned int	igd_rsvd : 5;
	unsigned int	igd_mbz : 3;
	unsigned int	igd_type : 5;
	unsigned int	igd_dpl : 2;
	unsigned int	igd_present : 1;
	unsigned short	igd_hoffset;
};

/* Note:
 *  Global girmask is used as a mask for interrupts that don't have a
 *  handler set. disable() & restore() are OR-ed with it to get the
 *  mask actually used.
 */
uint16	girmask;

#define	IMR1	0x21		/* Interrupt Mask Register #1		*/
#define	IMR2	0xA1		/* Interrupt Mask Register #2		*/

#define	ICU1	0x20		/* I/O port address, 8259A #1		*/
#define	ICU2	0xA0		/* I/O port address, 8258A #2		*/

#define	OCR	ICU1		/* Operation Command Register		*/
#define	IMR	(ICU1+1)	/* Interrupt Mask Register		*/

#define	EOI	0x20		/* non-specific end of interrupt	*/

#define NID		48	/* Number of interrupt descriptors	*/
#define	IGDT_TRAPG	15	/* Trap Gate				*/
#define	IGDT_INTRG	0xe	/* Interrupt Gate			*/

void	_8259_setirmask(void);	/* Set interrupt mask			*/

struct	int_entry int_actions[MAX_EXT_IRQS];

extern	struct	idt idt[NID];	/* Interrupt descriptor table		*/
extern	long	defevec[];	/* Default exception vector		*/

/* Local APIc registers */
volatile struct lapic_csreg *lapic = (struct lapic_csreg *)
						LAPIC_BASE_ADDR;

/*------------------------------------------------------------------------
 * initevec  -  Initialize exception vectors to a default handler
 *------------------------------------------------------------------------
 */
int32	initevec()
{
	int	i;

	for(i = 0; i < MAX_EXT_IRQS; i++) {
		int_actions[i].nitems = 0;
	}

	/* Set default exception vectors */

	for(i = 0; i < NID; i++) {
		set_evec(i, defevec[i]);
	}

	/* Load the interrupt descriptor table */

	lidt();

	/* Mask off all interrupts in legacy controller */

	girmask = 0xfffb;	/* Leave bit 2 enabled for IC cascade */

	/* Initialize the 8259A interrupt controllers */
	
	/* Master device */
	outb(ICU1, 0x11);	/* ICW1: icw4 needed		*/
	outb(ICU1+1, 0x20);	/* ICW2: base ivec 32		*/
	outb(ICU1+1, 0x4);	/* ICW3: cascade on irq2	*/
	outb(ICU1+1, 0x1);	/* ICW4: buf. master, 808x mode */
	outb(ICU1, 0xb);	/* OCW3: set ISR on read	*/

	/* Slave device */
	outb(ICU2, 0x11);	/* ICW1: icw4 needed		*/
	outb(ICU2+1, 0x28);	/* ICW2: base ivec 40		*/
	outb(ICU2+1, 0x2);	/* ICW3: slave on irq2		*/
	outb(ICU2+1, 0xb);	/* ICW4: buf. slave, 808x mode	*/
	outb(ICU2, 0xb);	/* OCW3: set ISR on read	*/

	_8259_setirmask();

	/* Mask all interrupt inputs in IO APIC */

	for(i = 0; i < 24; i++) {
		*((uint32 *)IOAPIC_IDX_ADDR) = 0x10 + (2 * i);
		*((uint32 *)IOAPIC_WIN_ADDR) |= 0x00010000;
	}

        return OK;
}

/*------------------------------------------------------------------------
 * set_evec  -  Set exception vector to point to an exception handler
 *------------------------------------------------------------------------
 */
int32	set_evec(uint32 xnum, uint32 handler)
{
	struct	idt	*pidt;

	pidt = &idt[xnum];
	pidt->igd_loffset = handler;
	pidt->igd_segsel = 0x8;		/* Kernel code segment */
	pidt->igd_mbz = 0;
	pidt->igd_type = IGDT_INTRG;
	pidt->igd_dpl = 0;
	pidt->igd_present = 1;
	pidt->igd_hoffset = handler >> 16;

        return OK;
}

/*------------------------------------------------------------------------
 * set_ivec  -  Set a high level interrupt handler
 *------------------------------------------------------------------------
 */
int	set_ivec (
		uint32	inum,			/* Interrupt number	*/
		void	*handler,		/* Interrupt handler	*/
		int32	arg			/* Handler argument	*/
		)
{
	struct	int_entry *ient;	/* Interrupt action entry	*/
	struct	int_info *iinfo;	/* Interrupt item entry		*/
	intmask	mask;			/* Saved interrupt mask		*/

	/* Sanity check on interrupt number */

	if( (inum < IRQBASE) || (inum >= (IRQBASE + MAX_EXT_IRQS)) ) {
		return SYSERR;
	}

	mask = disable();

	ient = &int_actions[inum-IRQBASE];

	/* If maximum no. of handlers already registered, return error */

	if(ient->nitems >= MAX_IRQ_SHARING) {
		restore(mask);
		return SYSERR;
	}

	iinfo = &ient->int_items[ient->nitems++];

	/* Add the interrupt handler and its argument */

	iinfo->int_handler = (void (*)(int32))handler;
	iinfo->int_arg = arg;

	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * ioapic_irq2vec  -  Map input irq to a vector
 *------------------------------------------------------------------------
 */
int32	ioapic_irq2vec (
		int32	irq,
		int32	vec
		)
{
	*((uint32 *)IOAPIC_IDX_ADDR) = 0x10 + (2 * irq) + 1;
	*((uint32 *)IOAPIC_WIN_ADDR) = 0;

	*((uint32 *)IOAPIC_IDX_ADDR) = 0x10 + (2 * irq);
	*((uint32 *)IOAPIC_WIN_ADDR) = 0x0000a000 | vec;

	return OK;
}

/*------------------------------------------------------------------------
 * int_dispatch  -  Dispatcher function for high level interrupt handlers
 *------------------------------------------------------------------------
 */
void	int_dispatch (
		int32	inum,		/* Interrupt number	*/
		long	*savedsp	/* Saved stack pointer	*/
		)
{
	struct	int_entry *ient;	/* Interrupt action entry	*/
	struct	int_info *iinfo;	/* Interrupt item entry		*/
	int32	i;			/* Loop index			*/

	lapic->eoi = 0;

	/* Sanity check on interrupt number */

	if( (inum < IRQBASE) || (inum >= (IRQBASE + MAX_EXT_IRQS)) ) {
		return;
	}

	ient = &int_actions[inum-IRQBASE];

	if(ient->nitems == 0) {
		/* This call never returns */
		trap(inum, savedsp);
	}

	/* Call all the registered high level interrupt handlers */

	for(i = 0; i < ient->nitems; i++) {

		iinfo = &ient->int_items[i];
		iinfo->int_handler(iinfo->int_arg);
	}

	/* Acknowledge interrupt in local APIC */

	//lapic->eoi = 0;
}

/*------------------------------------------------------------------------
 * _8259_setirmask  -  Set the interrupt mask in the controller
 *------------------------------------------------------------------------
 */
void	_8259_setirmask(void)
{
	if (girmask == 0) {	/* Skip until girmask initialized */
		return;
	}
	outb(IMR1, girmask&0xff);
	outb(IMR2, (girmask>>8)&0xff);
	return;
}

char *inames[] = {
	"divided by zero",
	"debug exception",
	"NMI interrupt",
	"breakpoint",
	"overflow",
	"bounds check failed",
	"invalid opcode",
	"coprocessor not available",
	"double fault",
	"coprocessor segment overrun",
	"invalid TSS",
	"segment not present",
	"stack fault",
	"general protection violation",
	"page fault",
	"coprocessor error"
};

/*------------------------------------------------------------------------
 * trap  -  print debugging info when a trap occurrs
 *------------------------------------------------------------------------
*/
void	trap (
	int	inum,	/* Interrupt number	*/
	long	*sp	/* Saved stack pointer	*/
	)
{
	intmask mask;	/* Saved interrupt mask	*/
	long	*regs;	/* Pointer to saved regs*/

	/* Disable interrupts */

	mask = disable();

	/* Get the location of saved registers */

	regs = sp;

	/* Print the trap message */

	kprintf("Xinu trap!\n");
	if (inum < 16) {
		kprintf("exception %d (%s) currpid %d (%s)\n", inum,
			inames[inum], currpid, proctab[currpid].prname);
	} else {
		kprintf("exception %d currpid %d (%s)\n", inum, currpid,
			proctab[currpid].prname);
	}

	/* Adjust stack pointer to get debugging information 	*/
	/* 8 registers pushed in _Xint or _extint		*/

	sp = regs + 8;

	/* Print the debugging information related to interrupt	*/

	if (inum == 8 || (inum >= 10 && inum <= 14)) {
		kprintf("error code %08x (%u)\n", *sp, *sp);
		sp++;
	}

	kprintf("CS %X eip %X\n", *(sp + 1), *sp);
	kprintf("eflags %X\n", *(sp + 2));

	/* Dump the register values */

	sp = regs + 7;

	kprintf("register dump:\n");
	kprintf("eax %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ecx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("esp %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebp %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("esi %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edi %08X (%u)\n", *sp, *sp);
	sp--;

	panic("Trap processing complete...\n");
	restore(mask);
}
