.code16

.section .reset.text, "ax"
.globl _reset
_reset:
    ljmp $0xf000, $_start

.section .model, "a"
    .byte 0xff
    .byte 0x00

.section .entry.text, "ax"
.globl _start
_start:
    // Wait for SDRAM to initialize
    mov $0xfffc, %dx
    mov $1, %ax
1:
    in (%dx), %ax
    cmp $0x1, %ax
    jne 1b
    xor %ax, %ax
    xor %dx, %dx
    // Clear flags for C entry.
    push %ax
    popf

    mov %cs, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %cs:bios_ss
    movw $0xdffe, %cs:bios_sp
    movw $0xdffe, %sp
    jmp root

.section .text, "ax"

bios_ss:
    .word 0
bios_sp:
    .word 0
saved_ss:
    .word 0
saved_sp:
    .word 0

.macro int_handler function
    mov %ss, %cs:saved_ss
    mov %sp, %cs:saved_sp
    mov %cs:bios_ss, %ss
    mov %cs:bios_sp, %sp

    sub $2, %sp
    push %ds
    push %ax
    push %cx
    push %dx
    push %bx
    push %sp
    push %bp
    push %si
    push %di
    mov %sp, %bp
    push %bp

    mov %cs:saved_ss, %ds
    mov %cs:saved_sp, %bx
    mov 4(%bx), %bx
    mov %bx, 18(%bp)

    // Use SS as DS inside the BIOS for stack-local variables that are passed
    // by address.
    mov %ss, %bp
    mov %bp, %ds

    xor %ax, %ax
    push %ax
    popf

    call \function
    add $2, %sp
    mov %sp, %bp

    mov 18(%bp), %ax
    mov %cs:saved_ss, %ds
    mov %cs:saved_sp, %bx
    mov %ax, 4(%bx)

    pop %di
    pop %si
    pop %bp
    pop %sp
    pop %bx
    pop %dx
    pop %cx
    pop %ax
    pop %ds
    add $2, %sp

    mov %cs:saved_ss, %ss
    mov %cs:saved_sp, %sp

    iret
.endm

.globl int12_handler
int12_handler:
    mov $640, %ax
    iret

.globl int10_handler
int10_handler:
    int_handler int10_function

.globl int11_handler
int11_handler:
    int_handler int11_function

.globl int13_handler
int13_handler:
    int_handler int13_function

.globl int14_handler
int14_handler:
    int_handler int14_function

.globl int15_handler
int15_handler:
    int_handler int15_function

.globl int16_handler
int16_handler:
    int_handler int16_function

.globl int17_handler
int17_handler:
    int_handler int17_function

.globl int18_handler
int18_handler:
    int_handler int18_function

.globl int19_handler
int19_handler:
    int_handler int19_function

.globl int1a_handler
int1a_handler:
    int_handler int1a_function

.globl int1b_handler
int1b_handler:
    int_handler int1b_function

.globl int1c_handler
int1c_handler:
    int_handler int1c_function
