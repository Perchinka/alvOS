[BITS 16]
print16:
    pusha
.print_loop:
    mov al, [bx]     ; Load character
    cmp al, 0        ; End of string
    je .done
    mov ah, 0x0E     ; BIOS teletype function
    int 0x10         ; Print character
    inc bx
    jmp .print_loop
.done:
    popa
    ret

print16_nl:
    mov ah, 0x0E
    mov al, 0x0D     ; Carriage return
    int 0x10
    mov al, 0x0A     ; Line feed
    int 0x10
    ret

