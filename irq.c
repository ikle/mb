/*
 * Interrupt Core
 *
 * Copyright (c) 2022-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "irq.h"

void term_printf (const char *fmt, ...);

void irq_entry (struct pt_regs st)
{
	switch (st.trapno) {
	default:
		term_printf ("W: unhandled interrupt %d, err = %d\n",
			     st.trapno, st.err);
	}
}
