include(Verilator)

set_source_files_properties(${VERILATED_DPI_CPP} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-error")
add_library(verilator STATIC ${VERILATOR_LIB_SOURCES})

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                          ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/microassembler/uasm
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/microcode.us
                           ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                           ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/microcode/microcode.us
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/Microcode.v.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/microcode/MicrocodeTypes.v.templ)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Microcode.v PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}\\\"")

verilate(Core ${CMAKE_CURRENT_SOURCE_DIR}/Core.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v
         ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v
         ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v
         ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v
         ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v
         ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)
verilate(Fifo ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v)
verilate(ImmediateReader ${CMAKE_CURRENT_SOURCE_DIR}/ImmediateReader.v)
verilate(LoadStore ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v)
verilate(Microcode ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)
verilate(ModRMDecode ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v)
verilate(Prefetch ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v)
verilate(RegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v)
verilate(SegmentRegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v)
