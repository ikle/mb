#
# Programmable Interval Timer (i8253)
#
# Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

.ifndef ARCH_I386_PIT_INC
.set	ARCH_I386_PIT_INC, 1

.macro pit_init base=0x40 channel=0 mode=3 counter=0 bcd=0
	mov	$((\channel << 6 & 0xC0) | 0x30 | (\mode << 1 & 0x0E) | (\bcd & 1)), %al
	out	%al, $(\base + 3)
	mov	$(\counter >> 0 & 0xff), %al
	out	%al, $(\base + \channel)
	mov	$(\counter >> 8 & 0xff), %al
	out	%al, $(\base + \channel)
.endm

.macro pit_read base=0x40 channel=0
	outb_p	$(\channel << 6 & 0xC0), $(\base + 3)	# latch counter
	inb	$(\base + \channel), %al		# real lo byte
	xchg	%al, %ah
	inb	$(\base + \channel), %al		# read hi byte
	xchg	%al, %ah
.endm

.endif  # ARCH_I386_PIT_INC
# vi: set ft=asm :
