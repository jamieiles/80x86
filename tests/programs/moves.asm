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

    mov         bp, 0x1000
    mov         di, 0x0200
    mov         word [bp + di + 0x34], 0xabcd
    mov         ax, [bp + di + 0x34]
    mov         sp, 0x80
    push        ax
    int3
