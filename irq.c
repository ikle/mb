/*
 * Interrupt Core
 *
 * Copyright (c) 2022-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>

#include "arch/i386/io.h"
#include "irq.h"

void term_putc   (int c);
void term_printf (const char *fmt, ...);

void irq_entry (struct pt_regs st)
{
	static int ticks;

	switch (st.trapno) {
	case 32:
		if (++ticks >= 1000) {
			term_putc ('.');
			ticks = 0;
		}

		return;
	default:
		term_printf ("W: unhandled interrupt %d, err = %d\n",
			     st.trapno, st.err);
	}
}
