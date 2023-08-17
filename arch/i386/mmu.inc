/*
 * Memory Management Unit
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

.ifndef ARCH_I386_MMU_INC
.set	ARCH_I386_MMU_INC,  1

.set	PAGE_SIZE, 4096

.macro mmu_tables
	.section .bss.tables, "aw", @nobits
	.align  PAGE_SIZE
mmu_pd:				# PD
	.skip	PAGE_SIZE
mmu_l:				# PT, boot time low 4M 1:1 mapping
	.skip	PAGE_SIZE
mmu_mt0:			# PT, PMA map first page table
	.skip	PAGE_SIZE
mmu_m0:				# PMA map, first page
	.skip	PAGE_SIZE
.endm

.macro mmu_init
	#
	# mmu_l[i] = (i << 12) | 3 for i in 0..1023
	#
	mov	$mmu_l, %edi
	mov	$3, %eax
1:
	mov	%eax, (%edi)
	add	$4, %edi
	add	$0x1000, %eax
	cmp	$(mmu_l + PAGE_SIZE), %edi
	jle	1b

	movl	$(mmu_m0  + 0x3), mmu_mt0		# PMA map, first page

	movl	$(mmu_l   + 0x3), mmu_pd		# boot time low 4M
	movl	$(mmu_l   + 0x3), mmu_pd + 0x300 * 4	# kernel start area
	movl	$(mmu_mt0 + 0x3), mmu_pd + 0x3fd * 4	# PMA map first PT
	movl	$(mmu_pd  + 0x3), mmu_pd + 0x3ff * 4	# PT VAs

	mov	$(mmu_pd), %eax		# set PD
	mov	%eax, %cr3
	mov	%cr0, %eax
	bts	$0x1f, %eax		# enable paging
	mov	%eax, %cr0
.endm

.endif  # ARCH_I386_MMU_INC
# vi: set ft=asm :
