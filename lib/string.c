/*
 * Standard C Memory and String helpers
 *
 * Copyright (c) 2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

void *memset (void *data, int c, size_t len)
{
	unsigned char *p;

	for (p = data; len > 0; ++p, --len)
		*p = (unsigned char) c;

	return data;
}
