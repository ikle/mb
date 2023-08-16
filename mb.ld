ENTRY (_start)

SECTIONS {
	. = 1M;

	.boot BLOCK (4K) : {
		*(.multiboot)
		*(.text.boot)
	}

	. += 3072M;

	.text BLOCK (4K) : AT (ADDR (.text) - 3072M) {
		*(.text.idt)
		*(.text)
		*(.rodata)
		*(.rodata.*)
	}

	.data BLOCK (4K) : AT (ADDR (.data) - 3072M) {
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
