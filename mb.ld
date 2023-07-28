ENTRY (_start)

SECTIONS {
	/* load address at 1 MB */
	. = 1M;

	.text BLOCK (4K) : ALIGN (4K) {
		*(.multiboot)
		*(.text)
		*(.rodata)
	}

	.data BLOCK (4K) : ALIGN (4K) {
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

	/*
	 * The compiler may produce other sections, by default it will
	 * put them in a segment with the same name. Simply add stuff
	 * here as needed.
	 */
}
