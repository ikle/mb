/*
 * Physical Memory Address
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_PMA_H
#define ARCH_I386_PMA_H  1

#include <stdint.h>

void pma_init      (void);
void pma_add_range (uint32_t from, uint32_t to, int res);

uint32_t pma_alloc (void);		/* allocate one page */
uint32_t pma_ref   (uint32_t pma);
uint32_t pma_unref (uint32_t pma);

#endif  /* ARCH_I386_PMA_H */
