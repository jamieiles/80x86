include_directories(..)
include_directories(../../sim/cppmodel)

add_library(simtests OBJECT
	    TestFifo.cpp
	    TestMemory.cpp
	    TestModRM.cpp
	    TestRegisterFile.cpp)

add_executable(sim-unittest
               SimMain.cpp
	       $<TARGET_OBJECTS:instructions>
	       $<TARGET_OBJECTS:instructionsnohw>
	       $<TARGET_OBJECTS:simtests>)
target_link_libraries(sim-unittest
		      8086sim
		      gtest
		      gmock)

add_test(sim-unittest ./sim-unittest --gtest_color=yes)
set_tests_properties(sim-unittest PROPERTIES TIMEOUT 5)
set_tests_properties(sim-unittest PROPERTIES LABELS SoftwareCPU)
