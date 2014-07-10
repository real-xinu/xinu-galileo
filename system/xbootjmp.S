/* xbootjmp.s - xbootjmp */

		.text
		.globl	xbootjmp

/*------------------------------------------------------------------------
 * xbootjmp -  call is xbootjmp(start_addr)
 *------------------------------------------------------------------------
 */
xbootjmp:
	pushl	%ebp		/* push ebp onto stack */
	movl	%esp,%ebp	/* record current SP in ebp */

	/* Retrieve the branch to address */
	movl	8(%ebp), %ecx
	
	/* Clear eflags */
	pushl	$0
	popf
	
	/* Restore EAX and EBX as required by multiboot protocol*/
	movl	bootsign, %eax
	movl	bootinfo, %ebx 
	
	jmp 	*%ecx       /* Jump to the Xinu branch address */