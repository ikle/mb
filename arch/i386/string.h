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

static inline void *memset (void *dest, int c, size_t count)
{
	asm volatile (
		"cld; rep stosb"
		: "+D" (dest), "+c" (count)
		: "a" (c)
		: "cc", "memory"
	);

	return dest;
}

static inline void *memcpy (void *dest, const void *src, size_t count)
{
	asm volatile (
		"cld; rep movsb"
		: "+D" (dest), "+S" (src), "+c" (count)
		:
		: "cc", "memory"
	);

	return dest;
}

#endif  /* ARCH_I386_STRING_H */
