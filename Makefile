BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build

BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
DISK_IMG = $(BUILD_DIR)/bootable.img

.PHONY: all clean run

all: $(DISK_IMG)

# Build bootloader
$(BOOTLOADER_BIN):
	make -C $(BOOT_DIR)

# Build kernel
$(KERNEL_BIN):
	make -C $(KERNEL_DIR)

# Create bootable disk image
$(DISK_IMG): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	mkdir -p $(BUILD_DIR)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOTLOADER_BIN) of=$@ bs=512 count=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

# Clean everything
clean:
	rm -rf build/

# Run the image in QEMU
run: 
	qemu-system-i386 -drive format=raw,file=$(DISK_IMG)
