/* ctxsw.s - ctxsw (for x86) */

		.text
		.globl	ctxsw

/*------------------------------------------------------------------------
 * ctxsw -  call is ctxsw(&old_sp, &new_sp)
 *------------------------------------------------------------------------
 */
ctxsw:
		pushl	%ebp		/* push ebp onto stack		*/
		movl	%esp,%ebp	/* record current SP in ebp	*/
		pushfl			/* record flags			*/
		pushal			/* save general regs on stack	*/

		/* save old segment registers here, if multiple allowed */

		movl	8(%ebp),%eax	/* Get mem location in which to	*/
					/*  save the old process's SP	*/
		movl	%esp,(%eax)	/* save old process's SP	*/
		movl	12(%ebp),%eax	/* Get location from which to	*/
					/*  restore new process's SP	*/

		/* The next instruction switches from the old process's	*/
		/*  stack to the new process's stack.			*/

		movl	(%eax),%esp	/* pick up new process's SP	*/

		/* restore new seg. registers here, if multiple allowed */

		popal			/* restore general registers	*/
		movl	4(%esp),%ebp	/* pick up ebp before restoring	*/
					/*   interrupts			*/
		popfl			/* restore interrupt mask	*/
		add	$4,%esp		/* skip saved value of ebp	*/
		ret			/* return to new process	*/
