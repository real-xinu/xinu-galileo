/* eth_q_dispatch.S - eth_q_dispatch */

#include <icu.s>
		.text
		.globl	eth_q_dispatch
		.globl	eth_q_intr
eth_q_dispatch:
		pushal
		pushfl
		cli
		movb	$EOI,%al	# clear the interrupt
		outb	%al,$OCW1_2
		movb	$EOI,%al
		outb	%al,$OCW2_2

		call	eth_q_intr

		popfl
		popal
		iret
