#
# Global Descriptor Table
#
# Copyright (c) 2022-2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

.ifndef ARCH_I386_GDT_INC
.set	ARCH_I386_GDT_INC, 1

.macro gdt_header
	.section .data.gdt
	.align	8
gdt:
.endm

.macro gdt_entry base=0 limit=0 access=0 flags=0
	.section .data.gdt
	.align	8

	.word	(\limit & 0xffff)
	.word	(\base  & 0xffff)
	.word	(\base  >> 16 & 0x00ff) | (\access << 8 & 0xff00)
	.word	(\limit >> 16 & 0x000f) | (\flags  << 4 & 0x00f0) | (\base  >> 16 & 0xff00)
.endm

.macro load_gdt base=$gdt limit=$gdt_limit cs=$0x08 ds=$0x10
	movw	\limit, -6(%esp)
	movl	\base,  -4(%esp)
	lgdt	-6(%esp)

	mov	\ds, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss
	jmp	\cs, $1f
1:
.endm

.macro gdt_trailer
	.section .data.gdt
	.align	8
	.set	gdt_limit, . - gdt - 1
.endm

.endif  # ARCH_I386_GDT_INC
# vi: set ft=asm :
