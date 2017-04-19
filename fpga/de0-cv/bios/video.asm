video_services:
    push bp
    mov bp, sp

    push dx
    push cx
    push bx
    push ax
    push si
    push di

    ; Set carry flag for error
    or word [bp+6], 1
    cmp ah, (end_video_service_table - video_service_table) / 2
    jg .bad_call
    mov cl, 7
    shr ax, cl
    mov bx, ax
    and bx, 0xfe
    clc
    call [cs:video_service_table + bx]
    jmp .out

.bad_call:
    call unimplemented_video
.out:
    jc .out_no_error
    and word [bp+6], 0xfffe
.out_no_error:
    pop di
    pop si
    pop ax
    pop bx
    pop cx
    pop dx

    pop bp
    iret

video_service_table:
    dw unimplemented_video           ; 0x0
    dw unimplemented_video           ; 0x1
    dw unimplemented_video           ; 0x2
    dw unimplemented_video           ; 0x3
    dw unimplemented_video           ; 0x4
    dw unimplemented_video           ; 0x5
    dw unimplemented_video           ; 0x6
    dw unimplemented_video           ; 0x7
    dw unimplemented_video           ; 0x8
    dw unimplemented_video           ; 0x9
    dw unimplemented_video           ; 0xa
    dw unimplemented_video           ; 0xb
    dw unimplemented_video           ; 0xc
    dw unimplemented_video           ; 0xd
    dw teletype_output               ; 0xe
    dw unimplemented_video           ; 0xf
end_video_service_table:

teletype_output:
    mov ax, [bp-8]
    call putchar
    clc
    ret
.tty: db `TTY\0`

unimplemented_video:
    push si
    mov si, .invalid
    call putstr
    pop si

    ret
.invalid: db `unimplemented video call\n\r\0`
