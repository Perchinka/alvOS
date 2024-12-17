bits 32

extern kernel_main

section .text
global _start

_start:
    mov esp, stack         ; Set up stack pointer
    and esp, -16           ; Align stack to 16 bytes
    mov eax, 0xDEADBEEF    ; Debug value
    push esp
    push eax
    cli
    call kernel_main       ; Call kernel_main (defined in kernel.c)

; IDT Load function
global idt_load
idt_load:
    mov eax, [esp + 4]     ; Load IDT pointer
    lidt [eax]             ; Load IDT
    ret

; Macros for ISRs with/without error codes
%macro ISR_NO_ERR 1
global _isr%1
_isr%1:
    cli
    push dword 0           ; Push dummy error code
    push dword %1          ; Push ISR index
    jmp isr_common
%endmacro

%macro ISR_ERR 1
global _isr%1
_isr%1:
    cli
    push dword %1          ; Push ISR index (error code should be already on stack)
    jmp isr_common
%endmacro

; Generate ISRs
ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_NO_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31
ISR_NO_ERR 32
ISR_NO_ERR 33
ISR_NO_ERR 34
ISR_NO_ERR 35
ISR_NO_ERR 36
ISR_NO_ERR 37
ISR_NO_ERR 38
ISR_NO_ERR 39
ISR_NO_ERR 40
ISR_NO_ERR 41
ISR_NO_ERR 42
ISR_NO_ERR 43
ISR_NO_ERR 44
ISR_NO_ERR 45
ISR_NO_ERR 46
ISR_NO_ERR 47

; ISR Common handler
extern isr_handler
isr_common:
    pusha                   ; Push all general-purpose registers
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10            ; Load data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    cld                     ; Clear direction flag

    push esp                ; Pass stack pointer to isr_handler
    call isr_handler
    add esp, 4              ; Clean up stack

    pop gs
    pop fs
    pop es
    pop ds
    popa                    ; Restore general-purpose registers

    add esp, 8              ; Clean up pushed error code and ISR index
    iret

; Define the stack
section .bss
align 4
stack:
    resb 0x4000             ; Reserve 16KB for the stack
