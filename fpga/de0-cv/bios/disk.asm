disk_services:
    push bp
    mov bp, sp

    push dx ; -2
    push cx ; -4
    push bx ; -6
    push ax ; -8
    push si ; -10
    push di ; -12

    ; Set carry flag for error
    or word [bp+6], 1
    cmp ah, (end_disk_service_table - disk_service_table) / 2
    jg .bad_call
    mov cl, 7
    shr ax, cl
    mov bx, ax
    and bx, 0xfe
    clc
    call [cs:disk_service_table + bx]
    jmp .out

.bad_call:
    call unimplemented
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
.disk: db `disk\n\r\0`

disk_service_table:
    dw disk_reset               ; 0x0
    dw disk_status              ; 0x1
    dw disk_read                ; 0x2
    dw unimplemented            ; 0x3
    dw unimplemented            ; 0x4
    dw unimplemented            ; 0x5
    dw unimplemented            ; 0x6
    dw unimplemented            ; 0x7
    dw disk_get_parameters      ; 0x8
    dw unimplemented            ; 0x9
    dw unimplemented            ; 0xa
    dw unimplemented            ; 0xb
    dw unimplemented            ; 0xc
    dw unimplemented            ; 0xd
    dw unimplemented            ; 0xe
    dw unimplemented            ; 0xf
    dw unimplemented            ; 0x10
    dw unimplemented            ; 0x11
    dw unimplemented            ; 0x12
    dw unimplemented            ; 0x13
    dw unimplemented            ; 0x14
    dw disk_get_type            ; 0x15
end_disk_service_table:

disk_reset:
disk_status:
    cmp dl, 0
    je .valid
    stc
    ret
.valid:
    mov byte [bp-7], 0
    clc
    ret

disk_read:
    cmp dl, 0
    je .valid
    push si
    mov si, .readinvalid
    call putstr
    pop si
    mov byte [bp-7], 0x80
    stc
    jmp .out

.valid:
    ; lba := (((cylinder * 2) + head) * 0x12) + (sector - 1)
    ;     := (((ch | ((cl & 0xc0) << 2)) + dh) * 0x12) + ((cl & 0x3f) - 1)
    mov di, [bp-6] ; Caller BX

    mov bx, [bp-4] ; Caller CX
    and bx, 0x3f
    dec bx

    ; DX contains the cylinder number, AL contains the head
    mov al, dh
    mov ah, 0

    mov dx, [bp-4]
    mov cl, 8
    ror dx, cl
    mov cl, 6
    shr dh, cl
    shl dx, 1

    add ax, dx
    mov cx, 0x12
    mul cx
    add ax, bx

    ; Move sector count from stored AL into CX
    mov cl, byte [bp-8]
    mov ch, 0
.read_loop:
    call read_sector

    add di, 512
    inc ax
    loop .read_loop

    mov byte [bp-7], 0 ; AH := 0
    clc
.out:
    ret
.readinvalid: db `read bad disk\0`

disk_get_parameters:
    cmp dl, 0
    je .valid
    stc
    jmp .out
.valid:
    mov byte [bp-7], 0
    mov byte [bp-6], 4
    mov word [bp-2], 1 ; DX
    mov word [bp-4], 0xffff

    push ax
    mov ax, cs
    mov es, ax
    mov word [bp-12], disk_base_table
    pop ax

    clc
.out:
    ret

disk_get_type:
    cmp dl, 0
    je .valid
    stc
    ret
.valid:
    mov word [bp-7], 1
    clc
    ret

disk_base_table:
    db 0 ; 0 step-rate
    db 0 ; 1 head load time
    db 0 ; 2 ticks to shutoff
    db 2 ; 3 512 bytes per sector
    db 0 ; 4 1 sector per track
    db 0 ; 5 inter block gap
    db 0 ; 6 data length
    db 0 ; 7 gap length
    db 0 ; 8 fill byte
    db 0 ; 9 head settle
    db 0 ; a motor startup time
