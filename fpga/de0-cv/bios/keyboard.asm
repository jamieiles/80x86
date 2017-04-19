keyboard_services:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 0x1

    cmp ah, 0x00
    je .keyboard_wait
    cmp ah, 0x10
    je .keyboard_enh_wait
    cmp ah, 0x01
    je .keyboard_status
    cmp ah, 0x02
    je .keyboard_shift_status
    cmp ah, 0x11
    je .keyboard_enh_keystroke

    push ax
    xchg ah, al
    call putbyte
    xchg ah, al
    call putbyte
    pop ax
    jmp .out

.keyboard_wait:
.keyboard_enh_wait:
    and word [bp+6], 0xfffe
    cmp byte [cs:.active_keypress], 0
    je .wait

    mov byte al, [cs:.active_keypress]
    mov byte [cs:.active_keypress], 0
    mov ah, al
    jmp .out

.wait:
    push dx
    mov dx, 0xfffa
.in_loop:
    in ax, dx
    test ah, 1
    jne .in_loop
    mov ah, al
    pop dx
    jmp .out
.keyboard_status:
.keyboard_enh_keystroke:
    and word [bp+6], ~(1 << 6)
    cmp byte [cs:.active_keypress], 0
    jne .out

    or word [bp+6], (1 << 6)
    mov byte [cs:.active_keypress], 0
    push dx
    mov dx, 0xfffa
    in ax, dx
    pop dx

    test ah, 1
    jz .out

    and word [bp+6], ~(1 << 6)
    mov ah, al
    mov [cs:.active_keypress], al
    test ah, 1
    jmp .out
.keyboard_shift_status:
    and word [bp+6], 0xfffe
    xor al, al
    jmp .out
.out:
    pop bp
    iret
.badkb: db `unhandled\0`

.active_keypress: db 0
