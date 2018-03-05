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

set(microprogram_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/aaa.us
    ${CMAKE_CURRENT_SOURCE_DIR}/aad.us
    ${CMAKE_CURRENT_SOURCE_DIR}/aas.us
    ${CMAKE_CURRENT_SOURCE_DIR}/adc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/add.us
    ${CMAKE_CURRENT_SOURCE_DIR}/and.us
    ${CMAKE_CURRENT_SOURCE_DIR}/bound.us
    ${CMAKE_CURRENT_SOURCE_DIR}/daa.us
    ${CMAKE_CURRENT_SOURCE_DIR}/das.us
    ${CMAKE_CURRENT_SOURCE_DIR}/arithmetic.us
    ${CMAKE_CURRENT_SOURCE_DIR}/call.us
    ${CMAKE_CURRENT_SOURCE_DIR}/comparison.us
    ${CMAKE_CURRENT_SOURCE_DIR}/cmp.us
    ${CMAKE_CURRENT_SOURCE_DIR}/cmps.us
    ${CMAKE_CURRENT_SOURCE_DIR}/div.us
    ${CMAKE_CURRENT_SOURCE_DIR}/enter.us
    ${CMAKE_CURRENT_SOURCE_DIR}/esc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/extend.us
    ${CMAKE_CURRENT_SOURCE_DIR}/flags.us
    ${CMAKE_CURRENT_SOURCE_DIR}/hlt.us
    ${CMAKE_CURRENT_SOURCE_DIR}/inc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/int.us
    ${CMAKE_CURRENT_SOURCE_DIR}/io.us
    ${CMAKE_CURRENT_SOURCE_DIR}/jmp.us
    ${CMAKE_CURRENT_SOURCE_DIR}/lds.us
    ${CMAKE_CURRENT_SOURCE_DIR}/lea.us
    ${CMAKE_CURRENT_SOURCE_DIR}/leave.us
    ${CMAKE_CURRENT_SOURCE_DIR}/les.us
    ${CMAKE_CURRENT_SOURCE_DIR}/lods.us
    ${CMAKE_CURRENT_SOURCE_DIR}/loop.us
    ${CMAKE_CURRENT_SOURCE_DIR}/mov.us
    ${CMAKE_CURRENT_SOURCE_DIR}/movs.us
    ${CMAKE_CURRENT_SOURCE_DIR}/mul.us
    ${CMAKE_CURRENT_SOURCE_DIR}/neg.us
    ${CMAKE_CURRENT_SOURCE_DIR}/not.us
    ${CMAKE_CURRENT_SOURCE_DIR}/or.us
    ${CMAKE_CURRENT_SOURCE_DIR}/pop.us
    ${CMAKE_CURRENT_SOURCE_DIR}/push.us
    ${CMAKE_CURRENT_SOURCE_DIR}/rcl.us
    ${CMAKE_CURRENT_SOURCE_DIR}/rcr.us
    ${CMAKE_CURRENT_SOURCE_DIR}/ret.us
    ${CMAKE_CURRENT_SOURCE_DIR}/rol.us
    ${CMAKE_CURRENT_SOURCE_DIR}/ror.us
    ${CMAKE_CURRENT_SOURCE_DIR}/sar.us
    ${CMAKE_CURRENT_SOURCE_DIR}/sbb.us
    ${CMAKE_CURRENT_SOURCE_DIR}/scas.us
    ${CMAKE_CURRENT_SOURCE_DIR}/shift.us
    ${CMAKE_CURRENT_SOURCE_DIR}/shl.us
    ${CMAKE_CURRENT_SOURCE_DIR}/shr.us
    ${CMAKE_CURRENT_SOURCE_DIR}/stos.us
    ${CMAKE_CURRENT_SOURCE_DIR}/sub.us
    ${CMAKE_CURRENT_SOURCE_DIR}/test.us
    ${CMAKE_CURRENT_SOURCE_DIR}/xchg.us
    ${CMAKE_CURRENT_SOURCE_DIR}/xlat.us
    ${CMAKE_CURRENT_SOURCE_DIR}/xor.us
    ${CMAKE_CURRENT_SOURCE_DIR}/wait.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode.us
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.us)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                          ${CMAKE_CURRENT_BINARY_DIR}/microcode.mif
                          ${CMAKE_CURRENT_BINARY_DIR}/Microcode.sv
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.sv
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/microassembler/uasm -I${CMAKE_CURRENT_BINARY_DIR}/../..
                           ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                           ${CMAKE_CURRENT_BINARY_DIR}/microcode.mif
                           ${CMAKE_CURRENT_BINARY_DIR}/Microcode.sv
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.sv
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.h
                           ${microprogram_sources}
                   DEPENDS ${microprogram_sources}
                           ${CMAKE_CURRENT_SOURCE_DIR}/Microcode.sv.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode.mif.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/MicrocodeTypes.sv.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/MicrocodeTypes.h.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/microassembler/uasm
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/microassembler/microasm/types.py)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Microcode.sv PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}\\\"")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(generate_microcode
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                  ${CMAKE_CURRENT_BINARY_DIR}/microcode.mif
                  ${CMAKE_CURRENT_BINARY_DIR}/Microcode.sv
                  ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.sv
                  ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.h)

verilate(TOPLEVEL Microcode
         VERILOG_SOURCES
         ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../Instruction.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../InsnDecoder.sv
         ${CMAKE_CURRENT_BINARY_DIR}/Microcode.sv)
