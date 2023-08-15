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

#define PMA_P		0x1000	/* first process PD page		*/
#define PMA_L		0x2000	/* low 4M with boot time 1:1 mapping	*/
#define PMA_K		0x3000	/* kernel start area 4M at 3G		*/
#define PMA_IL		0x4000	/* inital page pool, first page		*/
#define PMA_IH		0xF000	/* inital page pool, last page		*/

#define VMA_TOP		PAGE_L1_BASE

#endif  /* ARCH_I386_MMU_DEFS_H */
