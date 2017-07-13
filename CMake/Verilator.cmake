include_directories(${VERILATOR_INCLUDE_DIRS})

set(VERILATED_HEADERS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -ggdb3 -std=c++14")

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(VERILATOR_COVERAGE_FLAGS --coverage-line -Wno-style)
    set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} -DVM_COVERAGE=1")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VERILATOR_TRACE_FLAGS --trace --trace-underscore)
endif()

function(verilate)
    set(options "")
    set(oneValueArgs TOPLEVEL)
    set(multiValueArgs VERILOG_SOURCES GENERATED_SOURCES DEPENDS)
    cmake_parse_arguments(verilate "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(generated
        ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}.h
        ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}__Syms.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}__Syms.h)
    list(APPEND generated ${verilate_GENERATED_SOURCES})
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND generated
             ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}__Trace.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}__Trace__Slow.cpp)
    endif()
    foreach(source ${verilate_VERILOG_SOURCES})
        get_source_file_property(res ${source} COMPILE_FLAGS)
        if(NOT res STREQUAL "NOTFOUND")
            list(APPEND extra_compile_flags ${res})
        endif()
    endforeach(source)
    set(VERILATED_HEADERS "${VERILATED_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/V${verilate_TOPLEVEL}.h" )
    set(VERILATOR_INCLUDE_ARGS "")
    get_property(incdirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(incdir ${incdirs})
        set(VERILATOR_INCLUDE_ARGS "${VERILATOR_INCLUDE_ARGS} -I${incdir}")
    endforeach(incdir)
    separate_arguments(VERILATOR_INCLUDE_ARGS)
    set_source_files_properties(${generated} PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    add_custom_command(OUTPUT ${generated}
                       COMMAND rm -f ${generated}
                       COMMAND verilator -sv -O3 ${verilate_VERILOG_SOURCES}
                            -Wall -Wwarn-lint -Wwarn-style --assert
                            -I${CMAKE_CURRENT_SOURCE_DIR}
                            -I${CMAKE_CURRENT_BINARY_DIR}
                            ${VERILATOR_TRACE_FLAGS} ${VERILATOR_COVERAGE_FLAGS}
                            ${extra_compile_flags} --cc --top-module ${verilate_TOPLEVEL}
                            --Mdir ${CMAKE_CURRENT_BINARY_DIR}
                            ${VERILATOR_INCLUDE_ARGS}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       DEPENDS ${verilate_VERILOG_SOURCES} ${verilate_DEPENDS})
    add_library(V${verilate_TOPLEVEL} SHARED ${generated})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endfunction()
