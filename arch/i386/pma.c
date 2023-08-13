/*
 * Physical Memory Address
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stddef.h>

#include <arch/i386/mmu-defs.h>
#include <arch/i386/mmu.h>
#include <arch/i386/pma.h>

struct pma {
	struct pma *next;
	uint32_t refs, dev, off;
};

static struct pma *l2[PAGE_L2_COUNT], l1_0[PAGE_L1_COUNT], *free = NULL;

static void pma_push (struct pma *p, uint32_t pma)
{
	p->next = free;
	p->dev  = 1;  /* RAM device */
	p->off  = pma & ~PAGE_L0_MASK;

	free = p;
}

static void pma_reserve (struct pma *p)
{
	p->refs = 1;
	p->dev  = 1;  /* RAM device */
}

static void pma_reserve_lo (uint32_t pma)
{
	pma_reserve (l1_0 + (pma >> PAGE_L1_POS));
}

static void pma_push_lo (uint32_t pma)
{
	pma_push (l1_0 + (pma >> PAGE_L1_POS), pma);
}

void pma_init (void)
{
	uint32_t pma;

	l2[0] = l1_0;

	pma_reserve_lo (PMA_P);
	pma_reserve_lo (PMA_L);
	pma_reserve_lo (PMA_K);

	for (pma = PMA_IL; pma <= PMA_IH; pma += PAGE_L0_SIZE)
		pma_push_lo (pma);
}

#define PMA_DEFINE_ACCES					\
	const uint32_t i2 = pma >> PAGE_L2_POS & PAGE_L2_MASK;	\
	const uint32_t i1 = pma >> PAGE_L1_POS & PAGE_L1_MASK;	\
	struct pma *l1 = l2[i2];				\
	struct pma *l0 = l1 + i1;

void *malloc (size_t count);

static void pma_add_page (uint32_t pma, int res)
{
	PMA_DEFINE_ACCES

	if (l1 == NULL) {
		if ((l1 = malloc (sizeof (l1_0))) == NULL)
			return;  /* ENOMEM */

		l2[i2] = l1;
		l0 = l1 + i1;
	}

	if (l0->dev != 0)
		return;  /* EEXIST */

	if (res)
		pma_reserve (l0);
	else
		pma_push (l0, pma);
}

void pma_add_range (uint32_t from, uint32_t to, int res)
{
	from = (from + PAGE_L0_MASK) & ~PAGE_L0_MASK;  /* align forward  */
	to   = (to                 ) & ~PAGE_L0_MASK;  /* align backward */

	for (; from < to; from += PAGE_L0_SIZE)
		pma_add_page (from, res);
}

uint32_t pma_alloc (void)
{
	struct pma *p;

	if ((p = free) == NULL)
		return 0;

	free = p->next;
	p->next = NULL;
	p->refs = 1;
	return p->off;
}

uint32_t pma_ref (uint32_t pma)
{
	PMA_DEFINE_ACCES

	if (l1 == NULL || l0->dev == 0)
		return 0;  /* WTF? foreign page! EFAULT */

	return ++l0->refs;
}

uint32_t pma_unref (uint32_t pma)
{
	PMA_DEFINE_ACCES

	if (l1 == NULL || l0->dev == 0)
		return 0;  /* WTF? foreign page! EFAULT */

	if (--l0->refs == 0)
		pma_push (l0, pma);

	return l0->refs;
}
