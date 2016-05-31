cpu 8086

; 4KB bios image, vectors are located at an offset of 1KB
%macro vector 1
    dw          0x0400+%1
    dw          0xff00
%endmacro

segment code

start:
    mov         cx, 65
    lea         si, [cs:vector_table_start]
    rep         cs movsw 

    ; load the MBR
    mov         ah, 2h
    mov         al, 1h
    mov         bx, 7c00h
    int         0x13

    mov         dl, 0
    jmp         0000h:7c00h

bios_end:
    jmp         bios_end

vector_table_start:
%assign vecnum 0
%rep 32
    vector      vecnum
%assign vecnum vecnum + 1
%endrep
vector_table_end:

times 1024-($-$$) db 0xcc
times 32 iret

times 4096-16-($-$$) db 0xcc
    jmp         0xff00:0x0000
