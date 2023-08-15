/*
 * Standard C Memory and String helpers
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_STRING_H
#define ARCH_I386_STRING_H  1

#include <stddef.h>

static inline void *memset (void *dst, int c, size_t count)
{
	asm volatile (
		"rep stosb"
		: "+D" (dst), "+c" (count)
		: "a" (c)
		: "cc", "memory"
	);

	return dst;
}

static inline void *memcpy (void *dst, const void *src, size_t count)
{
	asm volatile (
		"rep movsb"
		: "+D" (dst), "+S" (src), "+c" (count)
		:
		: "cc", "memory"
	);

	return dst;
}

#endif  /* ARCH_I386_STRING_H */
