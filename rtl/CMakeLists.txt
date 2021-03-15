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

include(Verilator)

add_subdirectory(microcode)
set_source_files_properties(${VERILATOR_LIB_SOURCES} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-sign-compare -Wno-non-virtual-dtor -Wno-error")
set_source_files_properties(${VERILATOR_VCD_INCLUDE_DIR} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-sign-compare -Wno-non-virtual-dtor -Wno-error")
add_library(verilator SHARED ${VERILATOR_LIB_SOURCES})

set(ALU_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/aaa.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/aas.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/adc.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/add.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/and.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/bound.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/daa.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/das.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/enter.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/extend.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/flags.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/mul.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/or.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/not.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rcl.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rcr.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rol.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/ror.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/sar.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shift_flags.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shl.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shr.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/sub.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/xor.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/ALU.sv)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/InstructionDefinitions.sv
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/gen-instructions-functions
                           ${CMAKE_CURRENT_BINARY_DIR}/InstructionDefinitions.sv
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../documentation/instructions.yaml
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/InstructionDefinitions.sv.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/gen-instructions-functions)

add_custom_target(generate_instruction_definitions
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/InstructionDefinitions.sv)

set(CORE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/../config.v
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/MicrocodeTypes.sv
    ${ALU_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Instruction.sv
    ${CMAKE_CURRENT_BINARY_DIR}/InstructionDefinitions.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/InsnDecoder.sv
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/Microcode.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/FlagsEnum.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Core.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Divider.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Flags.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/ImmediateReader.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/IP.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/JumpTest.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/LoopCounter.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/PosedgeToPulse.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentOverride.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/TempReg.sv)

set_property(GLOBAL PROPERTY G_ALU_SOURCES "${ALU_SOURCES}")
set_property(GLOBAL PROPERTY G_CORE_SOURCES "${CORE_SOURCES}")

install(TARGETS verilator
        LIBRARY DESTINATION lib
        COMPONENT simulator)
