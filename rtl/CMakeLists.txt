include(Verilator)

add_subdirectory(microcode)
set_source_files_properties(${VERILATED_DPI_CPP} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-error")
add_library(verilator SHARED ${VERILATOR_LIB_SOURCES})

set(ALU_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/aaa.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/aas.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/adc.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/add.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/and.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/daa.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/das.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/flags.v
    ${CMAKE_CURRENT_SOURCE_DIR}/alu/mul.v
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

set(CORE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/MicrocodeTypes.v
    ${ALU_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/Microcode.v
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/MicrocodeTypes.v
    ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.v
    ${CMAKE_CURRENT_SOURCE_DIR}/FlagsEnum.v
    ${CMAKE_CURRENT_SOURCE_DIR}/Core.v
    ${CMAKE_CURRENT_SOURCE_DIR}/CSIPSync.v
    ${CMAKE_CURRENT_SOURCE_DIR}/Divider.v
    ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v
    ${CMAKE_CURRENT_SOURCE_DIR}/Flags.v
    ${CMAKE_CURRENT_SOURCE_DIR}/JumpTest.v
    ${CMAKE_CURRENT_SOURCE_DIR}/IP.v
    ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v
    ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v
    ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v
    ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentOverride.v
    ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v
    ${CMAKE_CURRENT_SOURCE_DIR}/TempReg.v)

set_property(GLOBAL PROPERTY G_ALU_SOURCES "${ALU_SOURCES}")
set_property(GLOBAL PROPERTY G_CORE_SOURCES "${CORE_SOURCES}")

install(TARGETS verilator
        LIBRARY DESTINATION lib
        COMPONENT simulator)
