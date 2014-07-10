/* ethDispatch.S - ethDispatch */

#include <icu.s>
		.text
		.globl	ethDispatch
		.globl	ethInterrupt
ethDispatch:
		pushal
		pushfl
		cli
		movb	$EOI,%al	# clear the interrupt
		outb	%al,$OCW1_2
		movb	$EOI,%al
		outb	%al,$OCW2_2

		call	ethInterrupt

		popfl
		popal
		iret
