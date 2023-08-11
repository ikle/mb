/*
 * Memory Management Unit: i386 Classic Paging
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_MMU_H
#define ARCH_I386_MMU_H

#include <stdint.h>

#define PMA_P	0x1000		/* first process PD page		*/
#define PMA_L	0x2000		/* low 4M with boot time 1:1 mapping	*/
#define PMA_K	0x3000		/* kernel start area 4M at 3G		*/
#define PMA_IL	0x4000		/* inital page pool, first page		*/
#define PMA_IH	0xF000		/* inital page pool, last page		*/

void mmu_init (void);

static inline void mmu_switch (uint32_t pd)
{
	asm volatile ("mov %0, %%cr3" :: "r" (pd));
}

static inline void mmu_flush (const void *vma)
{
	asm volatile ("invlpg (%0)" :: "r" (vma) : "memory");
}

#endif  /* ARCH_I386_MMU_H */
