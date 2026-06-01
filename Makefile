CC := gcc
XO := xorriso
QEMU := qemu-system-x86_64

KERNEL_SRC_DIR := kernel
BUILD_DIR      := build
KERNEL_OBJ_DIR := $(BUILD_DIR)/kernel
ISO_ROOT       := $(BUILD_DIR)/iso_root
LIMINE_DIR     := limine

KERNEL_ELF     := $(ISO_ROOT)/boot/kernel.elf
ISO_IMAGE      := myos.iso


CFLAGS := \
	-ffreestanding \
	-fno-pic \
	-mcmodel=kernel \
	-static \
	-no-pie \
	-nostdlib \

XFLAGS := \
    -as mkisofs \
    -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part \
    --efi-boot-image \
    --protective-msdos-label build/iso_root \
    -o $(ISO_IMAGE)

.PHONY: all clean init build run

all: build

clean: 
	rm -rf $(ISO_ROOT) $(ISO_IMAGE) $(KERNEL_ELF)

init:
	mkdir -p $(ISO_ROOT)/boot/$(LIMINE_DIR) $(ISO_ROOT)/EFI/BOOT

	cp $(LIMINE_DIR)/limine-bios.sys $(ISO_ROOT)/boot/$(LIMINE_DIR)
	cp $(LIMINE_DIR)/limine-bios-cd.bin $(ISO_ROOT)/boot/$(LIMINE_DIR)
	cp $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_ROOT)/boot/$(LIMINE_DIR)
	cp limine.conf $(ISO_ROOT)/boot/$(LIMINE_DIR)

	cp $(LIMINE_DIR)/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT

build: init
	$(CC) $(CFLAGS) -T linker.ld kernel.c kernel/libs/serial.c -o $(KERNEL_ELF)

	$(XO) $(XFLAGS)

	$(LIMINE_DIR)/limine.exe bios-install $(ISO_IMAGE)

run:
	$(QEMU) -cdrom $(ISO_IMAGE) -serial stdio -m 256M
