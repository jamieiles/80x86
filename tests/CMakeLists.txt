include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim/cpu)
include_directories(../sim)
include_directories(include)

add_executable(unittest
               SimMain.cpp
	       $<TARGET_OBJECTS:instructions>
	       $<TARGET_OBJECTS:simtests>)
target_link_libraries(unittest
		      8086sim
		      gtest
		      gmock)

add_test(unittest ./unittest --gtest_color=yes)
set_tests_properties(unittest PROPERTIES TIMEOUT 5)
add_subdirectory(programs)
add_subdirectory(instructions)
add_subdirectory(integration)
add_subdirectory(simulator)
add_subdirectory(rtl)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)
