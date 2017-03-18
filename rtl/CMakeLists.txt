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
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/int.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/jmp.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/mov.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/or.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/push.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/sbb.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/sub.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/test.us
    ${CMAKE_CURRENT_SOURCE_DIR}/microcode/xor.us
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

verilate(ALU ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
         ${CMAKE_CURRENT_SOURCE_DIR}/FlagsEnum.v
         ${CMAKE_CURRENT_SOURCE_DIR}/ALU.v)
verilate(Core ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
         ${CMAKE_CURRENT_SOURCE_DIR}/ALU.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Core.v
         ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Flags.v
         ${CMAKE_CURRENT_SOURCE_DIR}/IP.v
         ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v
         ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v
         ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v
         ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)
verilate(CSIPSync ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.v)
verilate(Fifo ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v)
verilate(Flags ${CMAKE_CURRENT_SOURCE_DIR}/Flags.v
         ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v)
verilate(ImmediateReader ${CMAKE_CURRENT_SOURCE_DIR}/ImmediateReader.v)
verilate(IP ${CMAKE_CURRENT_SOURCE_DIR}/IP.v)
verilate(LoadStore ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v)
verilate(Microcode ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)
verilate(ModRMDecode ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v)
verilate(Prefetch ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v)
verilate(RegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v)
verilate(SegmentRegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v)

install(TARGETS verilator
        LIBRARY DESTINATION lib)
