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
#include <arch/i386/vma.h>

static struct pma *free = NULL;

static uint32_t pma_address (struct pma *p)
{
	const struct pma *map = (void *) PMA_BASE;

	return (uint32_t) (p - map) << PAGE_L1_POS;
}

static void pma_free (struct pma *p)
{
	p->next = free;
	p->refs = 1;

	free = p;
}

static void pma_reserve (struct pma *p)
{
	p->next = NULL;
	p->refs = 2;
}

#define PMA_DEFINE_ACCES				\
	struct pma *map = (void *) PMA_BASE;		\
	struct pma *p   = map + (pma >> PAGE_L1_POS);

void pma_init (void)
{
	extern char _smap, _emap;  /* mapped kernel region */

	pma_add_range ((uint32_t) &_smap, (uint32_t) &_emap, 1);
	pma_add_range (PMA_RS, PMA_RE, 1);
	pma_add_range (PMA_FS, PMA_FE, 0);
}

static void pma_add_page (uint32_t pma, int res)
{
	PMA_DEFINE_ACCES

	if (!vma_mapped (p) && !vma_alloc_page_nc (p, 2))
		return;  /* ENOMEM */

	if (p->refs != 0)
		return;  /* EEXIST */

	if (res)
		pma_reserve (p);
	else
		pma_free (p);
}

#define PMA_ALIGN_UP(x)    (((x) + PAGE_L0_MASK) & ~PAGE_L0_MASK)
#define PMA_ALIGN_DOWN(x)  (((x)               ) & ~PAGE_L0_MASK)

void pma_add_range (uint32_t from, uint32_t to, int res)
{
	if (res) {
		from = PMA_ALIGN_DOWN (from);
		to   = PMA_ALIGN_UP   (to);
	}
	else {
		from = PMA_ALIGN_UP   (from);
		to   = PMA_ALIGN_DOWN (to);
	}

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
	p->refs = 2;
	return pma_address (p);
}

uint32_t pma_ref (uint32_t pma)
{
	PMA_DEFINE_ACCES

	if (!vma_mapped (p) || p->refs < 2)
		return 0;  /* EFAULT: foreign or free page */

	return ++p->refs;
}

uint32_t pma_unref (uint32_t pma)
{
	PMA_DEFINE_ACCES

	if (!vma_mapped (p) || p->refs < 2)
		return 0;  /* EFAULT: foreign or free page */

	if (--p->refs == 1)
		pma_free (p);

	return p->refs;
}
