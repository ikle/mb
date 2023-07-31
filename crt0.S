#
# Multiboot Trampoline
#
# Copyright (c) 2019-2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

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
	#	limit-lo, base-lo, access/base-mi, base-hi/flags/limit-hi
	#	  LLLL    BBBB    AABB    BFLL
	.word	0x0000, 0x0000, 0x0000, 0x0000
	#	base = 0, limit = 0xFFFFF, access = 0x9A, flags = 0xC
	.word	0xffff, 0x0000, 0x9a00, 0x00cf
	#	base = 0, limit = 0xFFFFF, access = 0x92, flags = 0xC
	.word	0xffff, 0x0000, 0x9200, 0x00cf
	#	base = 0, limit = 0xFFFFF, access = 0xFA, flags = 0xC
	.word	0xffff, 0x0000, 0xfa00, 0x00cf
	#	base = 0, limit = 0xFFFFF, access = 0xF2, flags = 0xC
	.word	0xffff, 0x0000, 0xf200, 0x00cf

.set	gdt_limit, . - gdt - 1

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

	call	main
	cli
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

