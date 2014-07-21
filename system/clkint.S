/* clkint.s - clkint (x86) */

/*------------------------------------------------------------------------
 *
 * clkint  -  interrupt dispatcher for clock interrupts
 *
 *------------------------------------------------------------------------
 */
#include <icu.s>
		.text
		.globl	clkint		# Clock interrupt dispatcher
clkint:
		pushal			# Save registers
		cli			# Disable further interrupts
		movb	$EOI,%al	# Reset interrupt
		outb	%al,$OCW1_2

		call	clkhandler	# Call high level handler

		sti			# Restore interrupt status
		popal			# Restore registers
		iret			# Return from interrupt
