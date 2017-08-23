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

    mov %cs, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    // Clear bss
    mov $bss_start, %di
    mov $bss_end, %cx
    sub $bss_start, %cx
    mov $0, %al
    rep stosb

    // Initialize rw data
    mov $data_start, %di
    mov $data_end, %cx
    sub $data_start, %cx
    mov $_data_load, %si
    rep movsb

    movw $stack_start, %sp

    jmp root

.section .data
bios_ss:
    .word 0xf000
bios_sp:
    .word stack_start

.section .bss
saved_ss:
    .word 0
saved_sp:
    .word 0

.section .text, "ax"
irq_entry:
    // Save current stack pointer, load BIOS stack
    mov %ss, %cs:saved_ss
    mov %sp, %cs:saved_sp
    mov %cs:bios_ss, %ss
    mov %cs:bios_sp, %sp

    // Save registers, setup stack frame
    push %ds
    pusha
    mov %sp, %bp
    push %bp

    // Load flags from iret frame, store in callregs
    mov %cs:saved_ss, %ds
    mov %cs:saved_sp, %bx
    mov 0(%bx), %ax
    mov 6(%bx), %bx
    mov %bx, 18(%bp)

    // Use SS as DS inside the BIOS for stack-local variables that are passed
    // by address
    mov %ss, %bp
    mov %bp, %ds

    call *%ax
    // Discard regs pointer parameter
    add $2, %sp
    mov %sp, %bp

    // Write possibly updated flags to the iret frame
    mov 18(%bp), %ax
    mov %cs:saved_ss, %ds
    mov %cs:saved_sp, %bx
    mov %ax, 6(%bx)

    // Restore all regs apart from flags - the iret frame has been updated
    // already
    popa
    pop %ds

    // Restore the callers stack
    mov %cs:saved_ss, %ss
    mov %cs:saved_sp, %sp
    add $2, %sp

    iret

.macro int_handler name, vector, function
.globl \name
\name:
    push $\function
    jmp irq_entry

.pushsection .rodata.vectors
    .align 4
    .word \vector
    .word \name
.popsection

.endm

int_handler int10_handler, 0x10, int10_function
int_handler int11_handler, 0x11, int11_function
int_handler int12_handler, 0x12, int12_function
int_handler int13_handler, 0x13, int13_function
int_handler int14_handler, 0x14, int14_function
int_handler int15_handler, 0x15, int15_function
int_handler int16_handler, 0x16, int16_function
int_handler int17_handler, 0x17, int17_function
int_handler int18_handler, 0x18, int18_function
int_handler int19_handler, 0x19, int19_function
int_handler int1a_handler, 0x1a, int1a_function
int_handler int1b_handler, 0x1b, int1b_function
int_handler int1c_handler, 0x1c, int1c_function
int_handler timer_handler, 0x08, timer_function
