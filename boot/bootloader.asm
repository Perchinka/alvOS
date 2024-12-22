[org 0x7c00]

KERNEL_OFFSET equ 0x1000 ; Hardcoded

mov [BOOT_DRIVE], dl 

mov bp, 0x9000 ; Just defining stack at 0x9000 
mov sp, bp

mov bx, MSG_16BIT_MODE
call print16
call print16_nl

call set_vga_mode
call load_kernel ; read the kernel from disk
call switch_to_32bit ; disable interrupts, load GDT,etc. 
jmp $

%include "print-16bit.asm"
%include "print-32bit.asm"
%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

[bits 16]
set_vga_mode:
    mov ah, 0x00    ; Function: Set video mode
    mov al, 0x13    ; Mode 13h (320x200, 256 colors)
    int 0x10        ; BIOS interrupt
    ret
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print16
    call print16_nl

    mov bx, KERNEL_OFFSET ; Read from disk and store at KERNEL_OFFSET
    mov dh, 40 ; Number of sectors to read
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_32BIT:
    mov ebx, MSG_32BIT_MODE
    call print32
    call KERNEL_OFFSET ; Give control to the kernel
    jmp $ 


BOOT_DRIVE db 0 ; It is a good idea to store it in memory because 'dl' may get overwritten
MSG_16BIT_MODE db "Started in 16-bit Real Mode", 0
MSG_32BIT_MODE db "Landed in 32-bit Protected Mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0

; padding
times 510 - ($-$$) db 0
dw 0xaa55
