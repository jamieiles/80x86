include_directories(${VERILATOR_INCLUDE_DIRS})

set(VERILATED_HEADERS)

set(CMAKE_CXX_FLAGS "-DVM_COVERAGE ${CMAKE_CXX_FLAGS} -O2 -ggdb3 -std=c++14")

macro(verilate sources toplevel)
    set(generated
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.h
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Syms.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Syms.h)
    set(VERILATED_HEADERS "${VERILATED_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.h" )
    add_custom_command(OUTPUT ${generated}
                       COMMAND verilator ${sources} --cc --top-module ${toplevel} --Mdir ${CMAKE_CURRENT_BINARY_DIR} -Wall -Wwarn-lint -Wwarn-style
                       DEPENDS ${sources})
    add_library(V${toplevel} STATIC ${generated} ${VERILATOR_LIB_SOURCES})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro()
message(${VERILATOR_INCLUDE_DIRS})
