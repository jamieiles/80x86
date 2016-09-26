include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim/cpu)
include_directories(../sim)
include_directories(include)

add_executable(unittest
               main.cpp
	       TestFifo.cpp
               TestMemory.cpp
               TestModRM.cpp
               TestRegisterFile.cpp)
target_link_libraries(unittest 8086sim gtest gmock instructions)

add_test(unittest ./unittest --gtest_color=yes)

add_subdirectory(programs)
add_subdirectory(instructions)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)

macro(system_test test_file)
    configure_file(${test_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    add_test(${test_file} python ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    set_property(TEST ${test_file} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_BINARY_DIR}/../python")
endmacro(system_test)

system_test(moves.py)
system_test(calls_and_jumps.py)
