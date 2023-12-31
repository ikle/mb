#
# Programmable Interrupt Controller (i8259A)
#
# Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

.ifndef ARCH_I386_PIC_INC
.set	ARCH_I386_PIC_INC, 1

.set	PIC_ICW1,	0x10
.set	PIC_ICW1_LTIM,	0x08		# level-triggered mode
.set	PIC_ICW1_SNGL,	0x02		# single mode
.set	PIC_ICW1_IC4,	0x01		# ICW4 follows

.set	PIC_ICW4_8086,	0x01		# x86 mode
.set	PIC_ICW4_AEOI,	0x02

.macro pic_out value=$0 port=$0x80
	mov	\value, %al
	out	%al, \port
	mov	$0, %al
	out	%al, $0x80
.endm

.macro pic_init base=0x20 ICW1=0 vector=8 ICW3=0 AEOI=0
	pic_out	$(PIC_ICW1 | PIC_ICW1_IC4 | \ICW1), $(\base)
	pic_out	$(\vector), $(\base + 1)

	.if !(\ICW1 & PIC_ICW1_SNGL)
		pic_out	$(\ICW3), $(\base + 1)
	.endif

	pic_out	$(PIC_ICW4_8086 | (\AEOI << 1 & 0x02)), $(\base + 1)
.endm

.endif  # ARCH_I386_PIC_INC
# vi: set ft=asm :
