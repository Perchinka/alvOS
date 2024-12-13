SRC_DIR = .
BUILD_DIR = ../build

ASM_SRC = bootloader.asm
ASM_BIN = $(BUILD_DIR)/bootloader.bin

ASM = nasm

.PHONY: all clean

all: $(ASM_BIN)

# Assemble the bootloader
$(ASM_BIN): $(SRC_DIR)/$(ASM_SRC)
	mkdir -p $(BUILD_DIR)
	$(ASM) -f bin -o $@ $<

# Clean build artifacts
clean:
	rm -f $(BUILD_DIR)/bootloader.bin
