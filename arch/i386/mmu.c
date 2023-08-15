/*
 * Memory Management Unit: i386 Classic Paging
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

#include <arch/i386/mmu-defs.h>
#include <arch/i386/mmu.h>

void mmu_init (void)
{
	uint32_t *P   = (void *) PMA_P,   *L  = (void *) PMA_L,
		 *MT0 = (void *) PMA_MT0, *M0 = (void *) PMA_M0,
		 *K   = (void *) PMA_K, i;

	memset (P,   0, PAGE_L0_SIZE);	/* PD				*/
	memset (MT0, 0, PAGE_L0_SIZE);	/* PT, PMA map first PT		*/
	memset (M0,  0, PAGE_L0_SIZE);	/* PMA map first page		*/
	memset (K,   0, PAGE_L0_SIZE);	/* PT, kernel start area	*/

	P[0x000] = PMA_L   | 0x3;	/* boot time low 4M 1:1 mapping	*/
	P[0x300] = PMA_K   | 0x3;	/* kernel start area 4M at 3G	*/
	P[0x3fd] = PMA_MT0 | 0x3;	/* PMA map first page table	*/
	P[0x3ff] = PMA_P   | 0x3;	/* PT VAs			*/

	for (i = 0; i < PAGE_L1_COUNT; ++i)
		L[i] = (i << PAGE_L1_POS) | 0x3;

	MT0[0x000] = PMA_M0 | 0x3;	/* PMA map first page		*/

	mmu_switch (PMA_P);
	asm volatile ("mov %%cr0, %%eax; bts $31, %%eax; mov %%eax, %%cr0"
		      ::: "eax", "cc", "memory");
}
