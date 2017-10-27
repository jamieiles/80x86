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

    call near near_call_target
    call 0:far_call_target
    mov ax, 0
    cmp ax, 0
    jne end
    mov word    [0x1004], 3
end:
    int3

near_call_target:
    mov word    [0x1000], 1
    ret

far_call_target:
    mov word    [0x1002], 2
    retf
