    ; Array address in DS:SI
    ; Length in CX
    ; SI+DX clobbered
put_hex_array:
    push ax

    cmp cx, 0
    jz .done
.print_loop:
    lodsb
    call putbyte
    loop .print_loop
.done:
    mov si, newline
    call putstr

    pop ax
    ret

putbyte:
    push ax
    push bx
    push cx
    mov bx, hex_table

    mov ah, al
    mov cl, 4
    shr al, cl
    cs xlat
    call putchar

    mov al, ah
    and al, 0xf
    cs xlat
    call putchar

    pop cx
    pop bx
    pop ax
    ret

dump_regs:
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    push si
    mov si, newline
    call putstr
    pop si

    mov cl, 8
    mov word ax, [bp-8]
    call putbyte
    shr ax, cl
    call putbyte

    mov word ax, [bp-6]
    call putbyte
    shr ax, cl
    call putbyte

    mov word ax, [bp-4]
    call putbyte
    mov byte al, [bp-3]
    call putbyte

    mov cl, 8

    mov ax, [bp-2]
    call putbyte
    shr ax, cl
    call putbyte

    mov ax, [bp-10]
    call putbyte
    shr ax, cl
    call putbyte

    mov ax, [bp-12]
    call putbyte
    shr ax, cl
    call putbyte

    mov ax, [bp]
    call putbyte
    shr ax, cl
    call putbyte

    mov si, newline
    call putstr

    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
