// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#include "macros.h"

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

    // Clear the BIOS stack space
    lea bios_start, %cx
    dec %cx
    mov %cx, %di
    xor %ax, %ax
    std
    rep stosb
    cld

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
bios_sw_sp:
    .word stack_start
bios_hw_sp:
    .word stack_start - 8192

.section .bss
saved_sw_ss: .word 0
saved_sw_sp: .word 0

.macro entry_fn name saved_ss_var saved_sp_var ss_var sp_var
.section .text, "ax"
.globl \name
\name:
    // Save current stack pointer, load BIOS stack
    mov %ss, %cs:\saved_ss_var
    mov %sp, %cs:\saved_sp_var
    mov %cs:\ss_var, %ss
    mov %cs:\sp_var, %sp

    // Save registers, setup stack frame
    sub $2, %sp
    push %es
    push %ds
    pusha
    mov %sp, %bp
    push %bp

    // Load flags from iret frame, store in callregs
    mov %cs:\saved_ss_var, %ds
    mov %cs:\saved_sp_var, %bx
    mov 0(%bx), %ax
    mov 6(%bx), %bx
    mov %bx, 20(%bp)

    // Use SS as DS inside the BIOS for stack-local variables that are passed
    // by address
    mov %ss, %bp
    mov %bp, %ds

    call *%ax
    // Discard regs pointer parameter
    add $2, %sp
    mov %sp, %bp

    // Write possibly updated flags to the iret frame
    mov 20(%bp), %ax
    mov %cs:\saved_ss_var, %ds
    mov %cs:\saved_sp_var, %bx
    mov %ax, 6(%bx)

    // Restore all regs apart from flags - the iret frame has been updated
    // already
    popa
    pop %ds
    pop %es
    add $2, %sp

    // Restore the callers stack
    mov %cs:\saved_ss_var, %ss
    mov %cs:\saved_sp_var, %sp
    add $2, %sp

    iret
.endm

entry_fn irq_entry, saved_sw_ss, saved_sw_sp, bios_ss, bios_sw_sp

stub_irq:
    iret

int_handler 0x1c, stub_irq

timer_irq:
    cli
    push %ds
    push %bx

    movw $0x40, %bx
    mov %bx, %ds
    xor %bx, %bx
    addw $1, 0x6c
    adcw $0, 0x6e

    // Handle 24 hour overflow flag
    cmpw $0x000f, 0x6c
    jne .Lno_overflow
    cmpw $0x0018, 0x6e
    jne .Lno_overflow

    movw $0, 0x6c
    movw $0, 0x6e
    incb 0x70

.Lno_overflow:
    pop %bx
    pop %ds

    int $0x1c

    push %ax
    movb $0x20, %al
    outb %al, $0x20
    pop %ax

    iret

int_handler 0x08, timer_irq

kbd_irq:
    push %ax
    movb $0x20, %al
    outb %al, $0x20
    pop %ax

    iret

int_handler 0x09, kbd_irq

.globl unused_int
unused_int:
    cld
    cli
    push $do_unused_int
    jmp irq_entry

.pushsection ".bios_date", "a"
    .ascii BIOS_BUILD_DATE
.popsection
