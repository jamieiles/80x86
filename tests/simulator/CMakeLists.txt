include_directories(..)

add_library(simtests OBJECT
	    TestFifo.cpp
	    TestMemory.cpp
	    TestModRM.cpp
	    TestRegisterFile.cpp)
