/* ttyDispatch.S - ttyDispatch */

#include <icu.s>
		.text
		.globl	ttyDispatch
		.globl	ttyInterrupt
ttyDispatch:
		pushal
		pushfl
		cli
		movb	$EOI,%al	# clear the interrupt
		outb	%al,$OCW1_2
		call	ttyInterrupt
		popfl
		popal
		iret
