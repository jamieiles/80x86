include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../tests/rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/Microcode.sv PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/\\\"")

get_property(ALU_SOURCES GLOBAL PROPERTY G_ALU_SOURCES)
get_property(CORE_SOURCES GLOBAL PROPERTY G_CORE_SOURCES)

verilate(TOPLEVEL RTLCPU
         VERILOG_SOURCES
         ${CMAKE_CURRENT_SOURCE_DIR}/RTLCPU.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/MemArbiter.sv
         ${CORE_SOURCES}
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VRTLCPU___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VRTLCPU___024unit.h
         DEPENDS generate_microcode)

add_library(rtlsim SHARED
            Core.cpp)

target_link_libraries(rtlsim
                      simcommon
                      verilator
                      VRTLCPU)
set_target_properties(rtlsim PROPERTIES LINK_FLAGS "-Wl,--export-dynamic")

install(TARGETS VRTLCPU
        LIBRARY DESTINATION lib
        COMPONENT simulator)
