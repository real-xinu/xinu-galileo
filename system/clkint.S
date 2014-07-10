/* clkint.s - clkint (x86) */

/*------------------------------------------------------------------------
 *
 * clkint  -  interrupt dispatcher for clock interrupts
 *
 *------------------------------------------------------------------------
 */

#include <icu.s>
		.text
count1000:	.word	1000		# Count 1000 ticks per second
		.globl	sltop		# Ptr to first sleep key
		.globl	clkint		# Clock interrupt dispatcher
clkint:
		pushal			# Save registers
		cli			# Disable further interrupts
		movb	$EOI,%al	# Reset interrupt
		outb	%al,$OCW1_2

		incl	ctr1000		# increment the ms counter
		subw	$1,count1000	# Decrement counter for 1 sec
		ja	cl1		# If ms remain, go to cl1
		incl	clktime		# Increment count of seconds
		movw	$1000,count1000	# Reset ms counter for next sec
cl1:
		cmpl	$0,slnonempty	# If no sleeping processes,
		je	clpreem		#   skip to preemption check
		movl	sltop,%eax	# Decrement key of first
		decl	(%eax)		#   sleeping process
		jg	clpreem		# Must use jg for signed int
		call	wakeup		# If zero, call wakeup
clpreem:	decl	preempt		# Decrement preemption counter
		jg	clret		# Must use jg for signed int
		call	resched		# If preemption, call resched
clret:					# Return from interrupt
		sti			# Restore interrupt status
		popal			# Restore registers
		iret			# Return from interrupt
