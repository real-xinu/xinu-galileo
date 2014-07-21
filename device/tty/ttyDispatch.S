/* ttyDispatch.S - ttyDispatch */

#include <icu.s>
		.text
		.globl	ttyDispatch
		.globl	ttyInterrupt

/*------------------------------------------------------------------------
 *  ttyDispatch  -  Dispatch an interrupt to the tty handler function
 *------------------------------------------------------------------------
 */
ttyDispatch:
		pushal			/* Save general-purpose regs.	*/
		pushfl			/* Save the flags register	*/
		cli			/* Prevent further interrupts	*/
		movb	$EOI,%al	/* Clear the interrupt		*/
		outb	%al,$OCW1_2
		call	ttyInterrupt	/* Call the handler		*/
		popfl			/* Restore the flags register	*/
		popal			/* Restore general-purpose regs.*/
		iret			/* Return from interrupt	*/
