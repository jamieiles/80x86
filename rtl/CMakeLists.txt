include(Verilator)

add_subdirectory(microcode)
set_source_files_properties(${VERILATED_DPI_CPP} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-error")
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

set(CORE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/MicrocodeTypes.sv
    ${ALU_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/microcode/Microcode.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/RegisterEnum.sv
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
