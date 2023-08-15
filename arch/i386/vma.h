/*
 * Virtual Memory Address
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_VMA_H
#define ARCH_I386_VMA_H  1

#include <stdbool.h>
#include <stdint.h>

void     vma_init   (void);
bool     vma_mapped (const void *vma);
uint32_t vma_to_pma (const void *vma);

bool vma_map_page_nc   (const void *vma, uint32_t pma, int mode);
void vma_unmap_page_nc (const void *vma);
bool vma_alloc_page_nc (void *vma, int mode);

bool vma_map_page   (const void *vma, uint32_t pma, int mode);
void vma_unmap_page (const void *vma);
bool vma_alloc_page (void *vma, int mode);

bool vma_map   (const void *vma, uint32_t pma, uint32_t count, int mode);
void vma_unmap (const void *vma, uint32_t count);
bool vma_alloc (void *vma, uint32_t count, int mode);

#endif  /* ARCH_I386_VMA_H */
