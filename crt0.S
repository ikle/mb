#
# Multiboot Trampoline
#
# Copyright (c) 2019-2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

.include "arch/i386/gdt.inc"
.include "arch/i386/pic.inc"
.include "arch/i386/pit.inc"

#
# Multiboot Header
#
.set MB_ALIGN,		1 << 0	# align loaded modules on page boundaries
.set MB_MEMINFO,	1 << 1	# provide memory map
.set MB_VIDEO,		1 << 2  # provide video mode table

.set MB_MAGIC,	0x1BADB002
.set MB_FLAGS,	MB_ALIGN | MB_MEMINFO | MB_VIDEO

.section .multiboot
.align 16
	.long	MB_MAGIC
	.long	MB_FLAGS
	.long	-(MB_MAGIC + MB_FLAGS)

#
# Global Descriptor Table
#
.section .data
.align 8
gdt:
	gdt_entry
	gdt_entry limit=0xFFFFF access=0x9A flags=0xC
	gdt_entry limit=0xFFFFF access=0x92 flags=0xC
	gdt_entry limit=0xFFFFF access=0xFA flags=0xC
	gdt_entry limit=0xFFFFF access=0xF2 flags=0xC

.set	gdt_limit, . - gdt - 1

#
# Interrupt Descriptor Table
#
.macro idt_gate index=0 emask=0x00007d00
	.section .text.gates
	.align	16
isr_\index:
	.if !(\emask & (1 << \index))
		push	$0
	.endif
	push	$\index
	jmp	isr_common
.endm

.macro idt_entry entry=0 cs=0x08 p=1 dpl=0 type=0xE
	.section .data
	.align	8

	.word	(\entry & 0xffff)
	.word	(\cs    & 0xffff)
	.word	(\p     << 15 & 0x8000) | (\dpl << 13 & 0x6000) | \
		(\type  <<  8 & 0x0f00)
	.word	(\entry >> 16 & 0xffff)
.endm

.section .data
	.align	8
idt:
.irp	index,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, \
	       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, \
	       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
	idt_gate \index
	idt_entry entry="(\index * 16 + 0x100010)"
.endr

.set	idt_limit, . - idt - 1

.section .text.gates
	.align	16
isr_common:
	pusha
	push	%ds
	push	%es
	push	%fs
	push	%gs

	mov	$0x10, %eax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs

	call	irq_entry

	pop	%gs
	pop	%fs
	pop	%es
	pop	%ds
	popa
	add	$8, %esp
	iret

.macro load_idt base=$idt limit=$idt_limit
	movw	\limit, -6(%esp)
	movl	\base,  -4(%esp)
	lidt	-6(%esp)
.endm

#
# Kernel Entry
#
.section .text
.align 16
.global _start
.type _start, @function
_start:
	movl	$stack_top, %esp
	push	%ebx

	cli
	load_gdt
	load_idt

	pic_init base=0x20 vector=0x20 ICW3=0x04 AEOI=1  # slave at IR2
	pic_init base=0xA0 vector=0x28 ICW3=2    AEOI=1  # slave id = 2

	pit_init channel=0 mode=3 counter=1193
	sti

	call	main
0:	hlt
	jmp	0b

# set the size of the _start symbol, this is useful when debugging
.size _start, . - _start

#
# Kernel Stack
#
.section .note.GNU-stack, "", @progbits

.section .stack, "aw", @nobits
.align 16
stack_bottom:
	.skip	16384	# 16 KB
stack_top:

