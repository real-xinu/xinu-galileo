/* sdmcDispatch.S - sdmcDispatch */

#include <icu.s>
		.text
		.globl	sdmcDispatch
		.globl	sdmcInterrupt
sdmcDispatch:
		pushal
		pushfl
		cli
		movb	$EOI,%al	# clear the interrupt
		outb	%al,$OCW1_2
		movb	$EOI,%al
		outb	%al,$OCW2_2

		call	sdmcInterrupt

		popfl
		popal
		iret
