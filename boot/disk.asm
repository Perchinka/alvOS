disk_load:
    push dx

    mov ah, 0x02    ; BIOS disk read function
    mov al, dh      ; Number of sectors to read
    ; Basically next three lines are magic lines to work with HDD
    mov ch, 0x00    ; cylinder number
    mov dh, 0x00    ; head number
    mov cl, 0x02    ; read from 2nd sector

    int 0x13        ; disk read interrupt
    
    jc disk_error   ; if carry set the error

    pop dx
    cmp dh, al      ; if disk read doesn't match raise error
    jne disk_error
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print16
    jmp $

DISK_ERROR_MSG:
    db 'Disk read error!', 0
