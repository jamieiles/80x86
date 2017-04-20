putstr:
    push si
    push ax
    push dx
    mov dx, 0xfffa
.out_loop:
    cs lodsb
    or al, al
    jz .done
    call putchar
    jmp .out_loop
.done:
    pop dx
    pop ax
    pop si
    ret

putchar:
    push dx
    push ax

    mov dx, 0xfffa
    out dx, al
    inc dx
.busy:
    in ax, dx
    test ah, 0x2
    jnz .busy

    pop ax
    pop dx

    ret
