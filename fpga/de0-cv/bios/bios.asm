cpu 8086

section .text vstart=0x0

_start:
    cld

    ; Clear vectors
    xor ax, ax
    mov es, ax
    mov cx, 256
    rep stosw

    ; Stack setup
    mov ax, cs
    mov es, ax
    mov ds, ax

    sub ax, 0x100
    mov ss, ax
    mov sp, 0x1000

    ; Initialize BIOS data area
    push ax
    push es
    push cx

    mov ax, 0x40
    mov es, ax
    xor ax, ax
    mov cx, 0xff
    rep stosb
    mov word [es:0x13], 640

    mov ax, 0xf000
    mov es, ax
    mov byte [es:0xfffe], 0xff

    pop cx
    pop es
    pop ax

    call install_vectors

    mov si, copyright
    call putstr

    call init_sd
    jmp load_and_boot

load_and_boot:
    push es
    xor ax, ax
    xor dx, dx
    mov di, 0
    mov es, di
    mov di, 0x7c00
    call read_sector
    pop es

    mov si, done
    call putstr

    mov dl, 0
    jmp 0x0000:0x7c00

%include "debug.asm"
%include "disk.asm"
%include "io.asm"
%include "keyboard.asm"
%include "sdcard.asm"
%include "video.asm"

install_vectors:
    mov si, vectors
    mov cx, end_of_vectors - vectors

    push es
    xor di, di
    mov es, di
    mov di, 0x10 * 4
    rep cs movsb
    pop es

    ret

equipment_list:
    push bp
    mov bp, sp

    ; Clear carry flag for no error
    and word [bp+6], 0xfffe
    mov ax, ((3 << 4) | (1 << 0))

    pop bp
    iret

memory_size:
    push bp
    mov bp, sp

    ; Clear carry flag for no error
    and word [bp+6], 0xfffe
    mov ax, 640

    pop bp
    iret

serial_services:
    push bp
    mov bp, sp

    ; No serial services
    and word [bp+6], 0xfffe

    pop bp
    iret

misc_system_services:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 1

    cmp ah, 0xc0
    je .system_configuration_parameters
    cmp ah, 0x41
    je .wait_event
    cmp ah, 0x88
    je .system_extended_memory_size
    cmp ah, 0x24
    je .a20_gate
    jmp .out

.system_configuration_parameters:
    or word [bp+6], 0x1
    mov ah, 0x80
    push ax
    mov ax, cs
    mov es, ax
    pop ax
    mov bx, bios_params_rec
    jmp .out
.system_extended_memory_size:
    and word [bp+6], 0xfffe
    xor ax, ax
    jmp .out
.wait_event:
    and word [bp+6], 0xfffe
    jmp .out
.a20_gate:
    or word [bp+6], 0x1
    jmp .out
.out:
    pop bp
    iret

printer_services:
    push bp
    mov bp, sp

    ; No printer services
    and word [bp+6], 0xfffe

    pop bp
    iret

basic_services:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 1

    push si
    mov si, .invalid
    call putstr
    pop si

    pop bp
    iret
.invalid: db `basic_services\n\r\0`
load_os:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 1

    push si
    mov si, .invalid
    call putstr
    pop si

    pop bp
    iret
.invalid: db `load_os\n\r\0`
rtc_services:
    push bp
    mov bp, sp

    ; Count of timer BIOS calls at 0:46c (32 bits)
    push ax
    push ds
    xor ax, ax
    mov ds, ax
    add word [0x46c], 1
    adc word [0x46e], 0
    pop ds
    pop ax

    ; Clear carry flag for no error
    and word [bp+6], 0xfffe

    mov ch, 0x00
    mov cl, 0x00
    mov dh, 0x00
    mov dl, 0x00
    mov ah, 0x00

    pop bp
    iret

break_handler:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 1

    push si
    mov si, .invalid
    call putstr
    pop si

    pop bp
    iret
.invalid: db `break_handler\n\r\0`
tick_handler:
    push bp
    mov bp, sp

    ; Set carry flag for error
    or word [bp+6], 1

    push si
    mov si, .invalid
    call putstr
    pop si

    pop bp
    iret
.invalid: db `tick_handler\n\r\0`

copyright:
    db `s80x86 DE0-CV BIOS, (C) Jamie Iles 2017\r\n\0`
hex_table:
    db `0123456789abcdef`
done:
    db `Booting from SD card...\n\r\0`
newline:
    db `\n\r\0`
boot_fail:
    db `ERROR: failed to read from boot device\r\n\0`
read_fail:
    db `ERROR: SD card read failed\r\n\0`
null_arg:
    dq 0
if_arg:
    dq 0xaa010000
ready_arg:
    dq 0x00000040
block_len_arg:
    dq 0x00020000

vectors:
    dw video_services, 0xff00           ; 0x10
    dw equipment_list, 0xff00           ; 0x11
    dw memory_size, 0xff00              ; 0x12
    dw disk_services, 0xff00            ; 0x13
    dw serial_services, 0xff00          ; 0x14
    dw misc_system_services, 0xff00     ; 0x15
    dw keyboard_services, 0xff00        ; 0x16
    dw printer_services, 0xff00         ; 0x17
    dw basic_services, 0xff00           ; 0x18
    dw load_os, 0xff00                  ; 0x19
    dw rtc_services, 0xff00             ; 0x1a
    dw break_handler, 0xff00            ; 0x1b
    dw tick_handler, 0xff00             ; 0x1c
    dw 0x0000, 0x0000                   ; 0x1d
    dw disk_base_table, 0xff00          ; 0x1e
    dw 0x0000, 0x0000                   ; 0x1f
end_of_vectors:


unimplemented:
    push si
    mov si, .invalid
    call putstr
    pop si
    stc
    ret
.invalid: db `unimplemented disk call\n\r\0`

bios_params_rec:
    dw (bios_params_end - bios_params_rec)
    db 0xff
    db 0x00
    db 0x00
    db 0x00
    db 0x00
    db 0x00
bios_params_end:

section .bss
spi_xfer_buf:
    resb 1200
