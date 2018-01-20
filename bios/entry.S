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
saved_ss: .word 0
saved_sp: .word 0
nest_count: .word 0

.section .text, "ax"
.globl irq_entry
irq_entry:
    incw %cs:nest_count
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

    cmpw $1, %cs:nest_count
    jne bad_nest

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

    decw %cs:nest_count
    iret

bad_nest:
    call error_bad_nest

stub_irq:
    iret

int_handler 0x1c, stub_irq

timer_irq:
    cli
    push %ds
    push %bx
    push %ax
    push %dx

    movw $0x40, %bx
    mov %bx, %ds
    movw $0x6c, %bx
    add $1, %ds:0(%bx)
    adc $0, %ds:2(%bx)

    int $0x1c

    movb $0x20, %al
    outb %al, $0x20

    pop %dx
    pop %ax
    pop %bx
    pop %ds

    iret

int_handler 0x08, timer_irq

.pushsection ".bios_date", "a"
    .ascii BIOS_BUILD_DATE
.popsection
