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

    ; An instruction with a modrm byte that writes to bp will implicitly use
    ; SS.  Make sure that subsequent instructions that don't have a modrm byte
    ; but default to DS don't think that they should be also using SS because
    ; BP was previously used.
    mov         bx, 0x2000
    mov         ss, bx
    mov         bx, 0x3000
    mov         ds, bx
    mov         bp, 0x100
    mov         word [bp + 0x100], 0xa5a5
    mov         ax, [bp + 0x100]
    and         ax, 0xa0a0
    mov         word [0x100], ax
    int3
