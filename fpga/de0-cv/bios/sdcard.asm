init_sd:
    call initial_clocks
    call enable_sd_cs

    ; Reset
    mov ax, 0x9540
    mov si, null_arg
    mov cx, 1
    call send_sd_command
    jnz .failure

    ; IF_COND
    mov ax, 0x8748
    mov si, if_arg
    mov cx, 1
    call send_sd_command
    jnz .failure

    ; Read OCR
    mov ax, 0x007a
    mov si, null_arg
    mov cx, 5
    call send_sd_command
    jnz .failure

    call sd_wait_ready
    jnz .failure

    ; Set block length
    mov ax, 0x0050
    mov si, block_len_arg
    mov cx, 1
    call send_sd_command
    jnz .failure

    ; Fast divider after initialization
    mov ax, 0x2
    mov dx, 0xfff0
    out dx, ax

    ret

.failure:
    mov si, boot_fail
    call putstr
    jmp $

    ; Read a sector from the SD card
    ; Sector number in AX
    ; Destination buffer address in ES:DI
read_sector:
    push bp
    mov bp, sp

    push es
    push di
    push dx
    push ax
    push cx

    mov cx, 512
    mul cx
    xchg ah, dl
    xchg al, dh
    mov [cs:.sector_address_buf], ax
    mov [cs:.sector_address_buf+2], dx

    ; Temporarily use the scratch buffer, so switch es to cs
    mov ax, cs
    mov es, ax

    mov ax, 0x0051
    mov si, .sector_address_buf
    mov cx, 1 + 1 + 512 + 2 + 512 ; r1, start token, data, CRC16

    call send_sd_command
    jz .copy
    mov si, read_fail
    call putstr
    jmp .out

.copy:
    ; Find start of data marker
    mov cx, 1 + 1 + 512 + 2 + 512
    mov al, 0xfe
    repne cs scasb

    ; Now copy to output buffer
    mov si, di
    mov cx, 512
    mov es, [bp-2]
    mov di, [bp-4]
    rep cs movsb

    clc

.out:
    pop cx
    pop ax
    pop dx
    pop di
    pop es

    pop bp
    ret

.sector_address_buf:
    dq 0

sd_wait_ready:
    mov ax, 0x0077
    mov si, null_arg
    mov cx, 1
    call send_sd_command
    jnz .out

    mov ax, 0x0069
    mov si, ready_arg
    mov cx, 1
    call send_sd_command
    jnz .out

    test byte [di - 1], 0x1
    jnz sd_wait_ready
.out:
    ret

enable_sd_cs:
    push ax
    push dx
    ; Enable CS, divider 0x1ff
    mov ax, 0x1ff
    mov dx, 0xfff0
    out dx, ax

    pop dx
    pop ax
    ret

    ; Send an SD card command
    ; al: command
    ; ah: crc
    ; [si]: arg
    ; cx: rx data len
    ;
    ; Returns the address of the response data in DI, ZF set on success
send_sd_command:
    push bp
    mov bp, sp

    push ax
    push cx

    mov di, spi_xfer_buf

    ; initial byte
    mov al, 0xff
    stosb
    ; cmd
    mov al, [bp - 2]
    stosb
    ; arg
    mov cx, 4
    rep cs movsb
    ; crc
    mov al, [bp - 1]
    stosb
    ; NCR bytes
    mov al, 0xff
    mov cx, [bp - 4]
    add cx, 8
    rep stosb

    pop cx

    add cx, 1 + 6 + 8 ; CMD + ARG + CRC + NCR
    mov si, spi_xfer_buf
    mov di, si

    call spi_xfer
    pop ax
    mov al, [es:di-1]
    test al, 0xfe

    mov sp, bp
    pop bp
    ret

    ; Transfer function, buffer in SI, length in CX, destination in DI
    ; CS should be asserted and the divider set
    ; DI left pointing to the first non-0xff byte
spi_xfer:
    push ax
    push cx
    push di
    mov dx, 0xfff2
.out_loop:
    cs lodsb
    out dx, al
.busy:
    in ax, dx
    test ax, 0x100
    jnz .busy
    stosb
    loop .out_loop

    pop di
    pop cx
    mov al, 0xff
    repe cs scasb

    pop ax

    ret

    ; Initial SD card reset
    ; Clobbers: di
initial_clocks:
    push ax
    push cx
    push dx

    ; Disable CS, divider 0x1ff
    mov ax, ((1 << 9) | 0x1ff)
    mov dx, 0xfff0
    out dx, ax

    mov cx, 8
    mov al, 0xff
    mov di, spi_xfer_buf
    rep stosb

    mov cx, 8
    mov si, spi_xfer_buf
    mov di, si
    call spi_xfer

    pop dx
    pop cx
    pop ax
    ret
