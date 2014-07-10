/* stacktrace.c - stacktrace */
#include <xinu.h>
static unsigned long	*esp;
static unsigned long	*ebp;

#define STKDETAIL

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
syscall stacktrace(int pid)
{
	struct procent	*proc = &proctab[pid];
	unsigned long	*sp, *fp;

	if (pid != 0 && isbadpid(pid))
		return SYSERR;
	if (pid == currpid) {
		asm("movl %esp,esp");
		asm("movl %ebp,ebp");
		sp = esp;
		fp = ebp;
	} else {
		sp = (unsigned long *)proc->prstkptr;
		fp = sp + 2; 		/* where ctxsw leaves it */
	}
	kprintf("sp %X fp %X proc->prstkbase %X\n", sp, fp, proc->prstkbase);
#ifdef STKDETAIL
	while (sp < (unsigned long *)proc->prstkbase) {
		for (; sp < fp; sp++)
			kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
		if (*sp == STACKMAGIC)
			break;
		kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
		fp = (unsigned long *) *sp++;
		if (fp <= sp) {
			kprintf("bad stack, fp (%08X) <= sp (%08X)\n", fp, sp);
			return SYSERR;
		}
		kprintf("RET  0x%X\n", *sp);
		sp++;
	}
	kprintf("STACKMAGIC (should be %X): %X\n", STACKMAGIC, *sp);
	if (sp != (unsigned long *)proc->prstkbase) {
		kprintf("unexpected short stack\n");
		return SYSERR;
	}
#endif
	return OK;
}
