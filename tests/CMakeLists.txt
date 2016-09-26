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
add_subdirectory(integration)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)
