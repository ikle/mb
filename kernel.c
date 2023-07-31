/*
 * Multiboot Sample Kernel
 *
 * Copyright (c) 2019-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "mb.h"

enum mc6848_reg {
	MC6848_CURSOR_HI	= 0x0e,
	MC6848_CURSOR_LO	= 0x0f,
};

enum vga_color {
	COLOR_BLACK,
	COLOR_BLUE,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_RED,
	COLOR_MAGENTA,
	COLOR_BROWN,
	COLOR_LIGHT_GREY,
	COLOR_DARK_GREY,
	COLOR_LIGHT_BLUE,
	COLOR_LIGHT_GREEN,
	COLOR_LIGHT_CYAN,
	COLOR_LIGHT_RED,
	COLOR_LIGHT_MAGENTA,
	COLOR_LIGHT_BROWN,
	COLOR_WHITE,
};

static unsigned short term_crti, term_width, term_height;
static unsigned short term_x, term_y;
static unsigned char term_color;
static uint16_t *term_frame;

static inline void outb (unsigned short port, unsigned char value)
{
	__asm__ __volatile__ ("outb %0, %1" :: "a" (value), "d" (port));
}

static void term_setcursor (void)
{
	const uint16_t loc = term_y * term_width + term_x;

	outb (term_crti, MC6848_CURSOR_HI);
	outb (term_crti + 1, (loc >> 8) & 0xff);

	outb (term_crti, MC6848_CURSOR_LO);
	outb (term_crti + 1, loc & 0xff);
}

void term_setcolor (enum vga_color fg, enum vga_color bg)
{
	term_color = fg | bg << 4;
}

static void put_at (int x, int y, int c)
{
	term_frame[y * term_width + x] = (c & 0xff | term_color << 8);
}

void term_clear (void)
{
	size_t x, y;

	term_x = 0;
	term_y = 0;

	for (y = 0; y < term_height; ++y)
		for (x = 0; x < term_width; ++x)
			put_at (x, y, ' ');

	term_setcursor ();
}

void term_init (void)
{
	term_crti   = *((uint16_t *) 0x463);
	term_width  = *((uint16_t *) 0x44a);
	term_height = *((uint8_t *)  0x484) + 1;
	term_frame  = (void *) 0xb8000;

	term_setcolor (COLOR_LIGHT_GREY, COLOR_BLACK);
	term_clear ();
}

static void term_newline (void)
{
	const size_t limit = term_width * (term_height - 1);
	size_t i;

	term_x = 0;

	if (++term_y < term_height) {
		term_setcursor ();
		return;
	}

	for (i = 0; i < limit; ++i)
		term_frame[i] = term_frame[i + term_width];

	term_y = term_height - 1;
	term_setcursor ();

	for (i = 0; i < term_width; ++i)
		put_at (i, term_y, ' ');
}

void term_putc (int c)
{
	switch (c) {
	case '\n':
		term_newline ();
		return;
	case '\r':
		term_x = 0;
		term_setcursor ();
		return;
	}

	put_at (term_x, term_y, c);

	if (++term_x < term_width) {
		term_setcursor ();
		return;
	}

	term_newline ();
}

void term_puts (const char *s)
{
	for (; *s != '\0'; ++s)
		term_putc (*s);
}

static void term_putn (unsigned long n)
{
	if (n >= 10)
		term_putn (n / 10);

	term_putc ('0' + n % 10);
}

static void term_put_xdigit (int x)
{
	term_putc (x < 10 ? '0' + x : 'a' - 10 + x);
}

static void term_putx (uint32_t x, int width)
{
	while (width-- > 0)
		term_put_xdigit ((x >> (width * 4)) & 0xf);
}

static void show_disk (uint32_t d)
{
	uint8_t p;

	if (((p = d >> 24) & 0x80) != 0)
		term_puts ("hd");
	else
		term_puts ("fd");

	term_putn (p & 0x7f);

	if ((p = d >> 16) != 0xff) {
		term_putc (',');
		term_putn (p);
	}

	if ((p = d >> 8) != 0xff) {
		term_putc (',');
		term_putn (p);
	}

	if ((p = d) != 0xff) {
		term_putc (',');
		term_putn (p);
	}
}

void term_printf (const char *fmt, ...)
{
	va_list ap;
	unsigned long n;
	const char *p;

	va_start (ap, fmt);

	for (; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			term_putc (*fmt);
			continue;
		}

		if (*++fmt == '\0')
			break;

		switch (*fmt) {
		case 's':
			p = va_arg (ap, void *);
			term_puts (p);
			break;
		case 'b':
		case 'w':
			n = va_arg (ap, unsigned);
			term_putn (n);
			break;
		case 'd':
			n = va_arg (ap, uint32_t);
			term_putn (n);
			break;
#if 0
		case 'q':
			n = va_arg (ap, uint64_t);
			term_putn (n);
			break;
#endif
		case 'x':
			n = va_arg (ap, uint32_t);
			term_putx (n, 8);
			break;
		case 'r':
			n = va_arg (ap, uint32_t);
			show_disk (n);
			break;
		default:
			term_putc (*fmt);
			break;
		}
	}

	va_end (ap);
}

static const char *e820_to_str (int type)
{
	switch (type) {
	case E820_RAM:		return "free";
	case E820_RESERVED:	return "reserved";
	case E820_ACPI:		return "ACPI";
	case E820_NVS:		return "NVS";
	case E820_UNUSABLE:	return "bad";
	case E820_DISABLED:	return "disabled";
	case E820_NVRAM:	return "nvram";
	case E820_OEM:		return "OEM";
	defualt:		return "unknown";
	}
}

void main (struct mb_info *mbi)
{
	term_init ();
	term_puts ("Hello, World!\nAnd another line of text.\n");

	term_printf ("cols = %w\nrows = %w\n", term_width, term_height);

	if ((mbi->flags & MBI_MEM) != 0) {
		term_printf ("mem lower = %d\n", mbi->mem_lower);
		term_printf ("mem upper = %d\n", mbi->mem_upper);
	}

	if ((mbi->flags & MBI_CMDLINE) != 0)
		term_printf ("cmdline = %s\n", (void *) mbi->cmdline);

	if ((mbi->flags & MBI_ROOT) != 0)
		term_printf ("disk = %r\n", mbi->root);

	if ((mbi->flags & MBI_FB) != 0) {
		term_printf ("fb addr   = %q\n", mbi->fb.addr);
		term_printf ("fb pitch  = %d\n", mbi->fb.pitch);
		term_printf ("fb width  = %d\n", mbi->fb.width);
		term_printf ("fb height = %d\n", mbi->fb.height);
		term_printf ("fb bpp    = %b\n", mbi->fb.bpp);
		term_printf ("fb type   = %b\n", mbi->fb.type);
	}

	if ((mbi->flags & MBI_MMAP) != 0) {
		void *p = (void *) mbi->mmap_addr, *end = p + mbi->mmap_len;
		struct mb_region *e;

		term_puts ("---------------- ---------------- --------\n"
			   "      base            length        type\n"
			   "---------------- ---------------- --------\n");

		for (e = p; p < end; p += sizeof (e->size) + e->size, e = p)
			term_printf ("%x%x %x%x %s\n",
				     e->base_hi, e->base_lo,
				     e->len_hi, e->len_lo,
				     e820_to_str (e->type));
	}
}
