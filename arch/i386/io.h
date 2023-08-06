/*
 * Input/Output Helpers
 *
 * Copyright (c) 2019-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ARCH_I386_IO_H
#define ARCH_I386_IO_H  1

#include <stdint.h>

static inline void outb (uint16_t port, uint8_t value)
{
	asm volatile ("outb %0, %1" :: "a" (value), "Nd" (port) : "memory");
}

static inline uint8_t inb (uint16_t port)
{
	uint8_t value;

	asm volatile ("inb %1, %0" : "=a" (value) : "Nd" (port) : "memory");

	return value;
}

static inline void io_wait (void)
{
	outb (0x80, 0);
}

static inline void outb_p (uint16_t port, uint8_t value)
{
	outb (port, value); io_wait ();
}

#endif  /* ARCH_I386_IO_H */
