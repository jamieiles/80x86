cpu 8086

    mov         bp, 0x1000
    mov         di, 0x0200
    mov         word [bp + di + 0x34], 0xabcd
    mov         ax, [bp + di + 0x34]
    mov         sp, 0x80
    push        ax
    int3
