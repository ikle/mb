/*
 * i386 Classic Paging Definitions
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_MMU_DEFS_H
#define ARCH_I386_MMU_DEFS_H  1

#define PAGE_L0_BITS	12u
#define PAGE_L1_BITS	10u
#define PAGE_L2_BITS	PAGE_L1_BITS

#define PAGE_L0_POS	0u
#define PAGE_L1_POS	(PAGE_L0_POS + PAGE_L0_BITS)
#define PAGE_L2_POS	(PAGE_L1_POS + PAGE_L1_BITS)

#define PAGE_L0_COUNT	(1u << PAGE_L0_BITS)
#define PAGE_L1_COUNT	(1u << PAGE_L1_BITS)
#define PAGE_L2_COUNT	(1u << PAGE_L2_BITS)

#define PAGE_L0_MASK	(PAGE_L0_COUNT - 1)
#define PAGE_L1_MASK	(PAGE_L1_COUNT - 1)
#define PAGE_L2_MASK	(PAGE_L2_COUNT - 1)

#define PAGE_L0_SIZE	(PAGE_L0_COUNT)
#define PAGE_L1_SIZE	(PAGE_L1_COUNT * 4)
#define PAGE_L2_SIZE	(PAGE_L2_COUNT * 4)

#define PAGE_L1_BASE	(0u - PAGE_L1_SIZE * PAGE_L2_COUNT)
#define PAGE_L2_BASE	(0u - PAGE_L2_SIZE)

#endif  /* ARCH_I386_MMU_DEFS_H */
