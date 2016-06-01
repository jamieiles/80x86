find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(../../sim/cpu)

add_library(Cpu SHARED
            Cpu.cpp)
set_target_properties(Cpu PROPERTIES PREFIX "")
target_link_libraries(Cpu 8086sim ${Boost_LIBRARIES})

file(COPY __init__.py
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
