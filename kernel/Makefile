# Directories
SRC_DIR = .
INCLUDE_DIR = include
BUILD_DIR = ../build

# Source files
C_SRCS = $(wildcard $(SRC_DIR)/*.c) \
         $(wildcard $(SRC_DIR)/drivers/*.c) \
         $(wildcard $(SRC_DIR)/sys/*.c)

ASM_SRCS = $(SRC_DIR)/entry_point.asm

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
       $(BUILD_DIR)/entry_point.o

# Output kernel binary
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
LINKER_SCRIPT = linker.ld

# Tools
CC = i686-elf-gcc
LD = i686-elf-ld
NASM = nasm

# Compilation flags
CFLAGS = -ffreestanding -m32 -O2 -g -I$(INCLUDE_DIR)
LDFLAGS = -T $(LINKER_SCRIPT) -m elf_i386
ASMFLAGS = -f elf32

# Phony targets
.PHONY: all clean

# Default target
all: $(KERNEL_BIN)

# Compile C source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble entry_point.asm
$(BUILD_DIR)/entry_point.o: $(SRC_DIR)/entry_point.asm
	mkdir -p $(dir $@)
	$(NASM) $(ASMFLAGS) $< -o $@

# Link all object files into the kernel binary
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ --oformat binary $^

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
