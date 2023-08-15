/*
 * Virtual Memory Address
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

#include <arch/i386/mmu-defs.h>
#include <arch/i386/mmu.h>
#include <arch/i386/pma.h>
#include <arch/i386/vma.h>

uint32_t vma_to_pma (const void *vma)
{
	VMA_DEFINE_ACCES

	if (l2[i2] == 0 || l1[i1] == 0)
		return 0;

	return (l1[i1] & ~PAGE_L0_MASK) + ((uint32_t) vma & PAGE_L0_MASK);
}

bool vma_map_page_nc (const void *vma, uint32_t pma, int mode)
{
	VMA_DEFINE_ACCES
	uint32_t prev;

	if (l2[i2] == 0 && !vma_alloc_page_nc (l1, 0x2))
		return false;  /* ENOMEM */

	if ((prev = l1[i1]) == 0)
		pma_ref (l2[i2]);

	pma_ref (pma);
	l1[i1] = (pma & ~PAGE_L0_MASK) | (mode & PAGE_L0_MASK) | 0x1;
	mmu_flush (vma);

	if (prev != 0)
		pma_unref (prev);

	return true;
}

void vma_unmap_page_nc (const void *vma)
{
	VMA_DEFINE_ACCES
	uint32_t pma;

	if (l2[i2] == 0 || l1[i1] == 0)
		return;

	pma = l1[i1];
	l1[i1] = 0;
	mmu_flush (vma);
	pma_unref (pma);

	if (pma_unref (l2[i2]) == 2) {  /* no more enties in L1? */
		pma = l2[i2];
		l2[i2] = 0;
		mmu_flush (l1);
		pma_unref (pma);
	}
}

bool vma_alloc_page_nc (void *vma, int mode)
{
	uint32_t pma;
	bool ok;

	if ((pma = pma_alloc ()) == 0)
		return false;

	ok = vma_map_page_nc (vma, pma, mode);
	pma_unref (pma);

	if (!ok)
		return false;

	memset (vma, 0, PAGE_L0_SIZE);
	return true;
}

static inline bool vma_validate_page (const void *vma, uint32_t pma)
{
	const uint32_t s = (uint32_t) vma;

	return (s & PAGE_L0_MASK) == 0 && (pma & PAGE_L0_MASK) == 0 &&
	       s < VMA_TOP;
}

bool vma_map_page (const void *vma, uint32_t pma, int mode)
{
	if (!vma_validate_page (vma, pma))
		return false;  /* EINVAL */

	return vma_map_page_nc (vma, pma, mode);
}

void vma_unmap_page (const void *vma)
{
	if (!vma_validate_page (vma, 0))
		return;  /* EINVAL */

	return vma_unmap_page_nc (vma);
}

bool vma_alloc_page (void *vma, int mode)
{
	if (!vma_validate_page (vma, 0))
		return false;  /* EINVAL */

	return vma_alloc_page_nc (vma, mode);
}

static inline bool vma_validate (const void *vma, uint32_t pma, uint32_t count)
{
	const uint32_t e = (uint32_t) vma + count * PAGE_L0_SIZE;

	return count != 0 && vma_validate_page (vma, pma) && e <= VMA_TOP;
}

bool vma_map (const void *vma, uint32_t pma, uint32_t count, int mode)
{
	const void *v;
	uint32_t i;

	if (!vma_validate (vma, pma, count))
		return false;  /* EINVAL */

	for (
		v = vma, i = 0;
		i < count;
		v += PAGE_L0_SIZE, pma += PAGE_L0_SIZE, ++i
	)
		if (!vma_map_page_nc (v, pma, mode))
			goto no_map;
	return true;
no_map:
	vma_unmap (vma, count);
	return false;
}

void vma_unmap (const void *vma, uint32_t count)
{
	if (!vma_validate (vma, 0, count))
		return;  /* EINVAL */

	for (; count > 0; vma += PAGE_L0_SIZE, --count)
		vma_unmap_page_nc (vma);
}

bool vma_alloc (void *vma, uint32_t count, int mode)
{
	void *p;
	uint32_t i;

	if (!vma_validate (vma, 0, count))
		return NULL;  /* EINVAL */

	if (!vma_alloc_page_nc (vma, mode))
		return false;

	for (i = 1, p = vma + PAGE_L0_SIZE; i < count; ++i, p += PAGE_L0_SIZE)
		if (!vma_alloc_page_nc (p, mode))
			goto no_alloc;
	return true;
no_alloc:
	vma_unmap (vma, count);
	return false;
}
