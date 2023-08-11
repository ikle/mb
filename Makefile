ASFLAGS  = -m32
CFLAGS   = -m32 -ffreestanding -fno-PIE -I$(CURDIR)
LDFLAGS  = -m32 -static -nostdlib -T mb.ld -Wl,-s -Wl,--build-id=none -no-pie
LDLIBS   = -lgcc

QEMU     = qemu-system-i386 -enable-kvm

.PHONY: all clean run resque

all: kernel

arch/i386/mmu.o: arch/i386/mmu-defs.h arch/i386/mmu.h
arch/i386/pma.o: arch/i386/mmu-defs.h arch/i386/mmu.h arch/i386/pma.h

crt0.o: arch/i386/gdt.inc arch/i386/idt.inc
crt0.o: arch/i386/pic.inc arch/i386/pit.inc

kernel: lib/string.o arch/i386/mmu.o
kernel: crt0.o kernel.c irq.c

clean:
	rm -f *.o kernel resque.img resque/boot/kernel

run: kernel
	$(QEMU) -kernel kernel
#	$(QEMU) -cdrom resque.img
#	$(QEMU) -hda resque.img

resque: kernel
	cp $^ resque/boot
	grub-mkrescue -o resque.img resque
