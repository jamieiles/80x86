cpu 8086

    times 1024 db 0

    mov word    [0x4], step_handler
    mov word    [0x10 * 4], dummy_handler

    mov         sp, 0x8000
    mov         bp, sp
    pushf
    or          word [bp-2], (1 << 8)
    popf

    int 0x10
    nop
    nop
    nop
end:
    int3

step_handler:
    inc bx
    iret

dummy_handler:
    inc cx
    iret
