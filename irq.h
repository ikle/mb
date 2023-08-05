/*
 * Interrupt Core
 *
 * Copyright (c) 2022-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef MARTEN_IRQ_H
#define MARTEN_IRQ_H  1

#include <stdint.h>

struct pt_regs {
	uint32_t gs, fs, es, ds;
	uint32_t di, si, bp, res0 /* new sp */, bx, dx, cx, ax;
	uint32_t trapno, err;
	uint32_t ip, cs, flags, sp, ss;
};

void irq_entry (struct pt_regs regs);

#endif  /* MARTEN_IRQ_H */
