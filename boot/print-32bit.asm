[BITS 32]
print32:
    pusha
    mov edx, 0xA0000 ; VGA memory
    mov cx, 0        ; Cursor offset
.print_loop:
    mov al, [ebx]    ; Load character
    cmp al, 0        ; End of string
    je .done
    mov [edx + ecx * 2], al ; Write character
    mov byte [edx + ecx * 2 + 1], 0x0F ; White on black
    inc cx
    inc ebx
    jmp .print_loop
.done:
    popa
    ret
