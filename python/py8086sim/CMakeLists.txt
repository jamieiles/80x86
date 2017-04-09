include(Verilator)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(../../sim/cppmodel)
include_directories(../../sim/RTLCPU)
include_directories(../../sim/common)
include_directories(../../tests/rtl)
include_directories(../../tests/jtag)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tests/rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../sim/RTLCPU)

add_library(Cpu SHARED
            Cpu.cpp)
set_target_properties(Cpu PROPERTIES PREFIX "")
target_link_libraries(Cpu 8086sim rtlsim JTAGCPU ${Boost_LIBRARIES})

file(COPY __init__.py
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
