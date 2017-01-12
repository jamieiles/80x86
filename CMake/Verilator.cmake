include_directories(${VERILATOR_INCLUDE_DIRS})

set(VERILATED_HEADERS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -ggdb3 -std=c++14")

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(VERILATOR_COVERAGE_FLAGS --coverage-line -Wno-style)
    set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} -DVM_COVERAGE=1")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VERILATOR_TRACE_FLAGS --trace --trace-underscore)
endif()

function(verilate toplevel sources)
    set(sources ${sources} ${ARGN})
    separate_arguments(sources)
    set(generated
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.h
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Syms.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Syms.h)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND generated
             ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Trace.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}__Trace__Slow.cpp)
    endif()
    set(VERILATED_HEADERS "${VERILATED_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/V${toplevel}.h" )
    set(VERILATOR_INCLUDE_ARGS "")
    get_property(incdirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(incdir ${incdirs})
        set(VERILATOR_INCLUDE_ARGS "${VERILATOR_INCLUDE_ARGS} -I${incdir}")
    endforeach(incdir)
    separate_arguments(VERILATOR_INCLUDE_ARGS)
    add_custom_command(OUTPUT ${generated}
                       COMMAND verilator -sv -O3 ${sources}
                            -Wall -Wwarn-lint -Wwarn-style
                            -I${CMAKE_CURRENT_SOURCE_DIR} ${VERILATOR_TRACE_FLAGS} ${VERILATOR_COVERAGE_FLAGS}
                            --cc --top-module ${toplevel}
                            --Mdir ${CMAKE_CURRENT_BINARY_DIR}
                            ${VERILATOR_INCLUDE_ARGS}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       DEPENDS ${sources})
    add_library(V${toplevel} STATIC ${generated})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endfunction()
