ENTRY (_start)

SECTIONS {
	/* load address at 1 MB */
	. = 1M;

	.boot BLOCK (4K) : ALIGN (4K) {
		*(.multiboot)
		*(.text.boot)
	}

	. += 0xC0000000;

	.text BLOCK (4K) : AT (ADDR (.text) - 0xC0000000) {
		*(.text.idt)
		*(.text)
		*(.rodata)
		*(.rodata.*)
	}

	.data BLOCK (4K) : AT (ADDR (.data) - 0xC0000000) {
		*(.data.gdt)
		*(.data.idt)
		*(.data)
	}

	.bss (NOLOAD) : {
		*(.bss)
		*(COMMON)
	}

	.stack (NOLOAD) : ALIGN (16) {
		*(.stack)
	}

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame*)
		*(.note.gnu.*)
	}
}
