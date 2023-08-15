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
