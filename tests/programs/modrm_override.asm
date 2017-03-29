cpu 8086

    times 1024 db 0

    ; An instruction with a modrm byte that writes to bp will implicitly use
    ; SS.  Make sure that subsequent instructions that don't have a modrm byte
    ; but default to DS don't think that they should be also using SS because
    ; BP was previously used.
    mov         bx, 0x2000
    mov         ss, bx
    mov         bx, 0x3000
    mov         ds, bx
    mov         bp, 0x100
    mov         word [bp + 0x100], 0xa5a5
    mov         ax, [bp + 0x100]
    and         ax, 0xa0a0
    mov         word [0x100], ax
    int3
