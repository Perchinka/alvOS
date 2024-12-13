OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_main)

SECTIONS {
    . = 0x1000;  /* Load address of the kernel */
    .text : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(COMMON)
        *(.bss)
    }
}
