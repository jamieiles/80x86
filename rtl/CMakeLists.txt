include(Verilator)

set_source_files_properties(${VERILATED_DPI_CPP} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-error")
add_library(verilator SHARED ${VERILATOR_LIB_SOURCES})

set(microprogram_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/adc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/add.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/and.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/arithmetic.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/call.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/comparison.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/cmp.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/esc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/extend.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/flags.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/inc.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/int.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/io.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/jmp.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/lds.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/lea.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/les.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/mov.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/neg.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/not.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/or.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/pop.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/prefixes.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/push.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/rcl.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/rcr.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/ret.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/rol.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/ror.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/sar.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/sbb.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/scas.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/shift.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/shl.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/shr.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/sub.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/test.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/xchg.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/xlat.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/xor.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/wait.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/microcode.us)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                          ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/microassembler/uasm
                           ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                           ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.h
                           ${microprogram_sources}
                   DEPENDS ${microprogram_sources}
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/Microcode.v.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/MicrocodeTypes.v.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/MicrocodeTypes.h.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/microassembler/uasm
                           ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/microassembler/microasm/types.py)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Microcode.v PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}\\\"")

set(ALU_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/adc.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/add.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/and.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/flags.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/or.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/not.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rcl.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rcr.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/rol.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/ror.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/sar.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shift_flags.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shl.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/shr.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/sub.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/xor.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/ALU.v)

verilate(TOPLEVEL ALU
         VERILOG_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
         ${CMAKE_CURRENT_SOURCE_DIR}/FlagsEnum.v
         ${ALU_SOURCES}
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VALU___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VALU___024unit.h)
verilate(TOPLEVEL Core
         VERILOG_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
         ${ALU_SOURCES}
         ${CMAKE_CURRENT_SOURCE_DIR}/Core.v
         ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Flags.v
         ${CMAKE_CURRENT_SOURCE_DIR}/IP.v
         ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v
         ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v
         ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentOverride.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v
         ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VCore___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VCore___024unit.h)
verilate(TOPLEVEL CSIPSync
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.v)
verilate(TOPLEVEL Fifo
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v)
verilate(TOPLEVEL Flags
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Flags.v
         ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v)
verilate(TOPLEVEL ImmediateReader
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ImmediateReader.v)
verilate(TOPLEVEL IP
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/IP.v)
verilate(TOPLEVEL LoadStore
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v)
verilate(TOPLEVEL Microcode
         VERILOG_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)
verilate(TOPLEVEL ModRMDecode
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v)
verilate(TOPLEVEL Prefetch
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v)
verilate(TOPLEVEL RegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v)
verilate(TOPLEVEL SegmentOverride
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentOverride.v)
verilate(TOPLEVEL SegmentRegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v)

install(TARGETS verilator VCore
        LIBRARY DESTINATION lib
        COMPONENT simulator)
