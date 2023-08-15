/*
 * i386 Classic Paging Definitions
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_MMU_DEFS_H
#define ARCH_I386_MMU_DEFS_H  1

#include <stdbool.h>
#include <stdint.h>

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

struct pma {
	struct pma *next;
	uint32_t refs;
};

#define PMA_COUNT	(PAGE_L2_COUNT * PAGE_L1_COUNT)
#define PMA_BASE	(PAGE_L1_BASE - sizeof (struct pma) * PMA_COUNT)

#define PMA_P		0x1000	/* first process PD page		*/
#define PMA_L		0x2000	/* low 4M with boot time 1:1 mapping	*/
#define PMA_MT0		0x3000	/* PMA map first page table		*/
#define PMA_M0		0x4000	/* PMA map first page			*/
#define PMA_K		0x5000	/* kernel start area 4M at 3G		*/

#define PMA_RS		0x1000	/* preallocated pages, start address	*/
#define PMA_RE		0x6000	/* preallocated pages, end address	*/

#define PMA_FS		0x6000	/* inital page pool, start address	*/
#define PMA_FE		0x10000	/* inital page pool, end address	*/

#define VMA_TOP		PMA_BASE

#define VMA_DEFINE_ACCES						    \
	const uint32_t i2 = (uint32_t) vma >> PAGE_L2_POS & PAGE_L2_MASK;   \
	const uint32_t i1 = (uint32_t) vma >> PAGE_L1_POS & PAGE_L1_MASK;   \
	uint32_t *l2 = (uint32_t *) PAGE_L2_BASE;			    \
	uint32_t *l1 = (uint32_t *) PAGE_L1_BASE + PAGE_L1_COUNT * i2;

static inline bool vma_mapped (const void *vma)
{
	VMA_DEFINE_ACCES

	return l2[i2] != 0 && l1[i1] != 0;
}

#endif  /* ARCH_I386_MMU_DEFS_H */
