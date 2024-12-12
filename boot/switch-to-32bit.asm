[bits 16]
switch_to_32bit:
    cli ; disable interrupts
    lgdt [gdt_descriptor] ; load the GDT descriptor
    mov eax, cr0
    or eax, 0x1 ; set 32-bit mode bit in cr0
    mov cr0, eax
    jmp CODE_SEG:init_32bit ; far jump by using a different segment

[bits 32]
init_32bit: ; we are now using 32-bit instructions
    mov ax, DATA_SEG ; update the segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000 ; update the stack right at the top of the free space
    mov esp, ebp

    call BEGIN_32BIT ; Call a well-known label with useful code
