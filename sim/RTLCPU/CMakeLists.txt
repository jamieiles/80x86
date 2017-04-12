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

verilate(TOPLEVEL Core
         VERILOG_SOURCES ${CORE_SOURCES}
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VCore___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VCore___024unit.h
         DEPENDS generate_microcode)

add_library(rtlsim SHARED
            Core.cpp)

target_link_libraries(rtlsim
                      simcommon
                      verilator
                      VCore)
set_target_properties(rtlsim PROPERTIES LINK_FLAGS "-Wl,--export-dynamic")

install(TARGETS VCore
        LIBRARY DESTINATION lib
        COMPONENT simulator)
