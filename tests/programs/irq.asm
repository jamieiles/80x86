cpu 8086

    times 1024 db 0

    mov         al, 0x13
    out         0x20, al
    mov         al, 0x08
    out         0x21, al
    mov         al, 0x01
    out         0x21, al

    mov         al, 0xff
    out         0x21, al

    mov word    [8 * 4], irq0_handler
    mov word    [9 * 4], irq1_handler
    sti

    mov         dx, 0xfff6
    mov         al, 1
    out         dx, al
    mov         al, 2
    out         dx, al
    nop
    nop
    nop
end:
    int3

irq0_handler:
    mov         al, 0x20
    out         0x20, al
    mov         ax, 0
    out         dx, ax
    mov         bx, 0x1234
    iret

irq1_handler:
    mov         al, 0x20
    out         0x20, al
    mov         ax, 0
    out         dx, ax
    mov         cx, 0x5678
    iret
