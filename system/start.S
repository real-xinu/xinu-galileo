/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)srt0.c	5.3 (Berkeley) 4/28/91
 */

/*
 * Startup code for standalone system
 * Non-relocating version -- for programs which are loaded by boot
 * Relocating version for boot
 * Small relocating version for "micro" boot
 */

#define	MULTIBOOT_HEADER_MAGIC  0x1BADB002
#define	MULTIBOOT_HEADER_FLAGS  0x00010003
#define MULTIBOOT_SIGNATURE	0x2BADB002	/* Multiboot signature verification	*/
#define MULTIBOOT_BOOTINFO_MMAP	0x00000040	/* mmap_length mmap_addr valid		*/
#define	GDT_ENTRIES		8
#define	GDT_ENTRY_SIZE		8
#define	GDT_BYTES		(GDT_ENTRIES * GDT_ENTRY_SIZE)
#define	IDT_ENTRIES		256
#define	IDT_ENTRY_SIZE		8
#define	IDT_BYTES		(IDT_ENTRIES * IDT_ENTRY_SIZE)
#define NULLSTK			(8192)		/* This must match NULLSTK defined in kernel.h 		*/

	.data

	.align	16	
	.globl	gdt
gdt:	.space	GDT_BYTES
gdtr:	.word	(GDT_BYTES-1)		# sizeof _gdt -1 (in bytes)
	.long	gdt			# global pointer to the gdt
		
	.align	16	        	
	.globl	idt
idt:	.space	IDT_BYTES       	
idtr:	.word	(IDT_BYTES-1)		# size of _idt -1 (in bytes)
	.long	idt			# global pointer to the idt

	.globl	cpudelay
cpudelay:	.long	1

	.text
	jmp	start			# Handle loaders that start at first byte of
					# text rather than the entry point
	.align 4
mbootheader:  				# Beginning of multiboot header, embedded in ELF header
	.long   MULTIBOOT_HEADER_MAGIC
	.long   MULTIBOOT_HEADER_FLAGS
	.long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	.long   mbootheader 		# Begin a.out kludge: header address
	.long   text			# load address
   	.long   bss			# load end address
	.long   ebss			# bss end address
	.long   start			# Entry address: end of multiboot header, end of a.out kludge

	.globl	start
start:

	/*
	 * Store the boot arguments.
	 */
	movl	%eax, bootsign
	movl	%ebx, bootinfo
	
	/*
	 * Test the multiboot signature and memory map flags
	 */
	cmpl	$MULTIBOOT_SIGNATURE, %eax
	jne	halt
	testb	$MULTIBOOT_BOOTINFO_MMAP, (%ebx)
	je	halt
	
	/*
	 * Iterate through the memory map segments to find the highest
	 *   address that will fit the null stack
	 */
	movl	0x30(%ebx), %ecx	# Load mmap_addr
	movl	0x2C(%ebx), %edx 	# Load mmap_length
	andl	$0xfffffffc, %edx	# Load mmap_end
	addl	%ecx, %edx
	movl	$0x0, %esp		# Initialize stack pointer to some small value

mmap_search_start:
	cmpl	%edx, %ecx		# If mmap_addr >= mmap_end then quit
	jae	mmap_search_done

	# skip memory segment if it is not usable
	movl	20(%ecx), %eax		# Test the address block type
	cmpl	$0x01, %eax		# If type is not equal to 1
	jne	mmap_search_next_sgmt	#   then this is not a usable block

	# skip memory segment if it is within the memory for Xinu
	movl	4(%ecx), %eax		# Load memory address of segment
	movl	12(%ecx), %ebx		# Load segment size
	add	%ebx, %eax		# Find end of memory segment
	movl	$end, %ebx		# Load end of xinu
	cmpl	%ebx, %eax		# Compare segment and xinu end
	jle	mmap_search_next_sgmt	# Skip if segment is within xinu
	
	# Segment is usable and not within xinu memory region
	#   Check if it is big enough to hold the null stack
	#   Subtract off any memory location that overlaps the
	#     the Xinu memory region
	movl	4(%ecx), %eax		# Load memory address of segment
	movl	$end, %ebx		# Load end of xinu
	subl	%eax, %ebx		# Determine overlap
	movl	12(%ecx), %eax		# Load segment size
	cmpl	$0x0, %ebx		# Check if overlap is greater than zero
	jl	mmap_search_no_overlap
	subl	%ebx, %eax		# Subtract overlap size
mmap_search_no_overlap:
	cmpl	$NULLSTK, %eax		# Compare with nullstack size
	jl	mmap_search_next_sgmt
	
	# Segment is big enough to hold the null stack
	#   want to save the highest possible address
	#   check if segment is larger than current stack pointer
	movl	4(%ecx), %eax		# Load memory address of segment
	movl	12(%ecx), %ebx		# Load segment size
	addl	%ebx, %eax		# Retrieve segment end
	cmpl	%esp, %eax		# Compare segment and stack pointer
	jl	mmap_search_next_sgmt	# Skip segment if less than
	
	# Segment is big enough to hold the nullstack
	#   and larger than the current stack pointer
	#   Use the end of the new segment as the null stack
	#     Round to the nearest block and subtract 4 (for STACKMAGIC)
	movl	%eax, %esp
	addl	$0x7, %esp
	andl	$0xFFFFFFF8, %esp
	subl	$0x4, %esp

mmap_search_next_sgmt:
	movl	(%ecx), %eax		# Load the entry_size
	addl	%eax, %ecx		# mmap_addr += entry_size + 4
	addl	$0x4, %ecx
	jmp	mmap_search_start
	
mmap_search_done:

	# Stack pointer set continue with boot
	movl	%esp, %ebp
	
	/*
	 * Clear flags.
	 */
	pushl	$0
	popf
	
	/*
	 * Zero the bss space
	 */
	movl	$ebss, %ebx
	movl	$bss, %ecx		# start of bss in %ecx
	subl	%ecx, %ebx		# bss size in %ebx
	pushl	%ebx
	pushl	%ecx
	call	sbzero
	popl	%ecx
	popl	%ebx
	
	/*
	 * Set up the global descriptor table.
	 */
	call	setsegs

	lgdt	gdtr
	/*
	 * Reload segment registers; load code segment by a far
	 * jump
	 */
	ljmp	$0x8, $gdt1	/* CS descriptor 1 */
gdt1:
	movl	$0x10, %eax	/* DS descriptor 2 */
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movl	$0x18, %eax	/* SS descriptor 3 */
	movw	%ax, %ss
	
	/*
	 * Call the nulluser to initialize the system
	 */
	call	nulluser
	call	halt

	.globl	inb
inb:	movl	4(%esp), %edx
	xorl	%eax, %eax	# clr eax
	inb	%dx, %al
	ret

	.globl	inw
inw:	movl	4(%esp), %edx
	xorl	%eax, %eax	# clr eax
	inw	%dx, %ax
	ret

	.globl  inl
inl:	movl	4(%esp), %edx
	xorl	%eax, %eax
	inl	%dx, %eax
	ret

	.globl	outb
outb:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outb	%al, %dx
	ret

	.globl	outw
outw:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outw	%ax, %dx
	ret

	.globl	outl
outl:	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outl	%eax, %dx
	ret

#ifndef SMALL
	.globl	_rtcin
_rtcin:	movl	4(%esp), %eax
	outb	%al, $0x70
	subl	%eax, %eax	# clr eax
	inb	$0x71, %al
	ret
#endif

	.globl ___udivsi3
___udivsi3:
	movl 4(%esp), %eax
	xorl %edx, %edx
	divl 8(%esp)
	ret

	.globl ___divsi3
___divsi3:
	movl 4(%esp), %eax
	xorl %edx, %edx
	cltd
	idivl 8(%esp)
	ret

	#
	# sbzero (base, cnt)
	#

	.globl sbzero
sbzero:
	pushl	%edi
	movl	8(%esp), %edi
	movl	12(%esp), %ecx
	movb	$0x00, %al
	cld
	rep
	stosb
	popl	%edi
	ret

	#
	# bcopy(src, dst, count)
	#

	.globl	bcopy
bcopy:
	pushl	%esi
	pushl	%edi
	movl	12(%esp), %esi
	movl	16(%esp), %edi
L1:
	movl	20(%esp), %ecx
	cld
	rep
	movsb
	popl	%edi
	popl	%esi
	ret

	# insw(port, addr, cnt)
	.globl	insw
insw:
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %edi
	movl	16(%esp), %ecx
	cld
	.byte 0x66, 0xf2, 0x6d	# rep insw
	movl	%edi, %eax
	popl	%edi
	ret

	# outsw(port, addr, cnt)
	.globl	outsw
outsw:
	pushl	%esi
	movw	8(%esp), %dx
	movl	12(%esp), %esi
	movl	16(%esp), %ecx
	cld
	.byte 0x66, 0xf2, 0x6f	# rep outsw
	movl	%esi, %eax
	popl	%esi
	ret

	#
	# lidt() - load interrupt descriptor table from idtr
	#
	.globl	lidt
lidt:
	lidt	idtr
	ret
