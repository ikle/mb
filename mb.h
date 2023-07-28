/*
 * Multiboot
 *
 * Copyright (c) 2019-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef MB_H
#define MB_H  1

#include <stdint.h>

enum mb_info_flag {
	MBI_MEM		= 1 << 0,
	MBI_ROOT	= 1 << 1,
	MBI_CMDLINE	= 1 << 2,
	MBI_MODS	= 1 << 3,
	MBI_SYMS	= 1 << 4,
	MBI_ELFSYMS	= 1 << 5,
	MBI_MMAP	= 1 << 6,
	MBI_DRIVES	= 1 << 7,
	MBI_CONF	= 1 << 8,
	MBI_LOADER	= 1 << 9,
	MBI_APM		= 1 << 10,
	MBI_VBE		= 1 << 11,
	MBI_FB		= 1 << 12,
};

struct mb_mod {
	uint32_t start, end, string, reserved;
};

struct mb_syms {
	uint32_t  tabsize, strsize, addr, reserved;
};

struct mb_elfsyms {
	uint32_t  num, size, addr, shndx;
};

enum e820_type {
	E820_RAM	= 1,
	E820_RESERVED	= 2,
	E820_ACPI	= 3,		/* ACPI reclaimable		*/
	E820_NVS	= 4,		/* ACPI NVS			*/
	E820_UNUSABLE	= 5,
	E820_DISABLED	= 6,
	E820_NVRAM	= 7,		/* Persistent memory		*/
	E820_OEM	= 12,
};

struct mb_region {
	uint32_t size;			/* entry size excl. this field	*/
	uint32_t base_lo, base_hi;	/* base address of region	*/
	uint32_t len_lo,  len_hi;	/* region size in bytes		*/
	uint32_t type;			/* E820 memory type		*/
};

struct mb_vbe {
	uint32_t control_info, mode_info, mode;
	uint32_t iface_seg, iface_off, iface_len;
};

enum mb_fb_type {
	MB_FB_INDEXED,
	MB_FB_DIRECT,
	MB_FB_TEXT,
};

struct mb_indexed {
	uint32_t palette_addr;	/* (u8 r, g, b)[size] */
	uint16_t palette_size;
};

struct mb_direct {
	uint8_t r_pos, r_len;
	uint8_t g_pos, g_len;
	uint8_t b_pos, b_len;
};

struct mb_fb {
	uint64_t addr;
	uint32_t pitch, width, height;
	uint8_t  bpp, type;
	union {
		struct mb_indexed indexed;
		struct mb_direct  direct;
	};
};

struct mb_info {
	uint32_t	flags;
	uint32_t	mem_lower, mem_upper;
	uint32_t	root;
	uint32_t	cmdline;
	uint32_t	mods_count, mods_addr;
	union {
		struct mb_syms    syms;
		struct mb_elfsyms elfsyms;
	};
	uint32_t	mmap_len, mmap_addr;
	uint32_t	drives_len, drives_addr;
	uint32_t	conf;
	uint32_t	loader;
	uint32_t	apm;
	struct mb_vbe	vbe;
	struct mb_fb	fb;
};

#endif  /* MB_H */
