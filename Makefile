ASFLAGS  = -m32
CFLAGS   = -m32 -ffreestanding -fno-PIE
LDFLAGS  = -m32 -static -nostdlib -T mb.ld -Wl,-s -Wl,--build-id=none -no-pie
LDLIBS   = -lgcc

QEMU     = qemu-system-i386 -enable-kvm

.PHONY: all clean run resque

all: kernel

crt0.o: arch/i386/gdt.inc arch/i386/pic.inc
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
