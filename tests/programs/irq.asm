; Copyright Jamie Iles, 2017
;
; This file is part of s80x86.
;
; s80x86 is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; s80x86 is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

cpu 8086

    times 1024 db 0

    mov         al, 0x13
    out         0x20, al
    mov         al, 0x08
    out         0x21, al
    mov         al, 0x01
    out         0x21, al

    ; Unmask the IRQs we're using (1+2)
    mov         al, 0xf9
    out         0x21, al

    mov word    [9 * 4], irq0_handler
    mov word    [10 * 4], irq1_handler
    sti

    mov         dx, 0xfff6
    mov         al, 2
    out         dx, al
    mov         al, 4
    out         dx, al
    nop
    nop
    nop
end:
    int3

irq0_handler:
    mov         al, 0x20
    out         0x20, al
    mov         ax, 0
    out         dx, ax
    mov         bx, 0x1234
    iret

irq1_handler:
    mov         al, 0x20
    out         0x20, al
    mov         ax, 0
    out         dx, ax
    mov         cx, 0x5678
    iret
