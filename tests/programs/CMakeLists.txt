# Copyright Jamie Iles, 2017
#
# This file is part of s80x86.
#
# s80x86 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# s80x86 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

include(Nasm)

nasm_file(OUTPUT moves.bin
          SOURCES moves.asm)
add_custom_target(tests_moves.asm ALL DEPENDS moves.bin)

nasm_file(OUTPUT calls_and_jumps.bin
          SOURCES calls_and_jumps.asm)
add_custom_target(tests_calls_and_jumps.asm ALL DEPENDS calls_and_jumps.bin)

nasm_file(OUTPUT modrm_override.bin
          SOURCES modrm_override.asm)
add_custom_target(tests_modrm_override.asm ALL DEPENDS modrm_override.bin)

nasm_file(OUTPUT irq.bin
          SOURCES irq.asm)
add_custom_target(tests_irq.asm ALL DEPENDS irq.bin)

nasm_file(OUTPUT singlestep.bin
          SOURCES singlestep.asm)
add_custom_target(tests_singlestep.asm ALL DEPENDS singlestep.bin)
