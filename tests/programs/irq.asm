cpu 8086

    times 1024 db 0

    mov word    [32 * 4], irq0_handler
    mov word    [33 * 4], irq1_handler
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
    mov         bx, 0x1234
    mov         al, 0
    out         dx, al
    iret

irq1_handler:
    mov         cx, 0x5678
    mov         al, 0
    out         dx, al
    iret
