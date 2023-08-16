ENTRY (_start)

SECTIONS {
	/* load address at 1 MB */
	. = 1M;

	.text BLOCK (4K) : ALIGN (4K) {
		*(.multiboot)
		*(.text.idt)
		*(.text)
		*(.rodata)
		*(.rodata.*)
	}

	.data BLOCK (4K) : ALIGN (4K) {
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
