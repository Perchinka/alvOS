BUILD_DIR = build
SRC_DIR = .

C_SOURCES = $(wildcard $(SRC_DIR)/kernel/*.c $(SRC_DIR)/drivers/*.c)
C_HEADERS = $(wildcard $(SRC_DIR)/kernel/*.h $(SRC_DIR)/drivers/*.h)

ASM_SOURCES = $(wildcard $(SRC_DIR)/boot/*.asm)
ASM_BINS = $(patsubst $(SRC_DIR)/boot/%.asm, $(BUILD_DIR)/boot/%.bin, $(ASM_SOURCES))

OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJ = $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/kernel/*.asm $(SRC_DIR)/drivers/*.asm))

CFLAGS = -m32 -fno-pie -fno-stack-protector -nostdlib -ffreestanding
LDFLAGS = -m elf_i386 -Ttext 0x1000 --oformat binary -e kernel_entry

all: os-image

run: all
	qemu-system-i386 -drive format=raw,file=os-image

os-image: $(BUILD_DIR)/boot/bootloader.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel/kernel_entry.o $(OBJ)
	ld $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(C_HEADERS)
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	nasm $< -f elf -o $@

$(BUILD_DIR)/boot/%.bin: $(SRC_DIR)/boot/%.asm
	mkdir -p $(dir $@)
	nasm $< -f bin -I $(SRC_DIR)/boot/ -o $@

clean:
	rm -rf $(BUILD_DIR) os-image
