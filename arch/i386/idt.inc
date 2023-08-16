#
# Interrupt Descriptor Table
#
# Copyright (c) 2022-2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

.ifndef ARCH_I386_IDT_INC
.set	ARCH_I386_IDT_INC, 1

.macro idt_header
	.section .data.idt
	.align	8
idt:
.endm

.macro idt_gate index=0 emask=0x00007d00
	.section .text.idt
	.align	16
idt_gate_\index:
	.if !(\emask & (1 << \index))
		push	$0
	.endif
	push	$\index
	jmp	idt_gate_common
.endm

.macro idt_entry entry=0 cs=0x08 p=1 dpl=0 type=0xE
	.section .data.idt
	.align	8

	.word	(\entry & 0xffff)
	.word	(\cs    & 0xffff)
	.word	(\p     << 15 & 0x8000) | (\dpl << 13 & 0x6000) | (\type  <<  8 & 0x0f00)
	.word	(\entry >> 16 & 0xffff)
.endm

.macro load_idt base=$idt limit=$idt_limit
	movw	\limit, -6(%esp)
	movl	\base,  -4(%esp)
	lidt	-6(%esp)
.endm

.macro idt_trailer
	.section .data.idt
	.align	8
	.set	idt_limit, . - idt - 1

	.section .text.idt
	.align	16
idt_gate_common:
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
.endm

.endif  # ARCH_I386_IDT_INC
# vi: set ft=asm :
