SRC_DIR = .
INCLUDE_DIR = include
BUILD_DIR = ../build

C_SRCS = $(wildcard $(SRC_DIR)/*.c) \
         $(wildcard $(SRC_DIR)/drivers/*.c) \
         $(wildcard $(SRC_DIR)/lib/*.c) \
         $(wildcard $(SRC_DIR)/sys/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
       $(patsubst $(SRC_DIR)/drivers/%.c, $(BUILD_DIR)/drivers/%.o, $(wildcard $(SRC_DIR)/drivers/*.c)) \
       $(patsubst $(SRC_DIR)/lib/%.c, $(BUILD_DIR)/lib/*.o, $(wildcard $(SRC_DIR)/lib/*.c)) \
       $(patsubst $(SRC_DIR)/sys/%.c, $(BUILD_DIR)/sys/%.o, $(wildcard $(SRC_DIR)/sys/*.c))

# Output kernel binary
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
LINKER_SCRIPT = linker.ld

CC = i686-elf-gcc
LD = i686-elf-ld

# Flags
CFLAGS = -ffreestanding -m32 -nostdlib -I$(INCLUDE_DIR)
LDFLAGS = -T $(LINKER_SCRIPT) -m elf_i386

.PHONY: all clean

all: $(KERNEL_BIN) $(OBJS)

# Compile each source file into an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link all object files into the kernel binary
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ --oformat binary $^

# Clean build artifacts
clean:
	rm -f $(OBJS) $(KERNEL_BIN)
