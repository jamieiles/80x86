include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim)

add_executable(unittest
               main.cpp
               Arithmetic.cpp
               Shift.cpp
               TestAdc.cpp
               TestAdd.cpp
               TestAscii.cpp
               TestCall.cpp
               TestCmp.cpp
               TestControl.cpp
               TestExtend.cpp
               TestFifo.cpp
               TestFlags.cpp
               TestJump.cpp
               TestIncDec.cpp
               TestInt.cpp
               TestIO.cpp
               TestLock.cpp
               TestLoop.cpp
               TestMemory.cpp
               TestMisc.cpp
               TestModRM.cpp
               TestMov.cpp
               TestMul.cpp
               TestNeg.cpp
               TestNot.cpp
               TestPointer.cpp
               TestPush.cpp
               TestRegisterFile.cpp
               TestRet.cpp
               TestSar.cpp
               TestSegment.cpp
               TestSbb.cpp
               TestShl.cpp
               TestShr.cpp
               TestString.cpp
               TestSub.cpp
               TestTrap.cpp
               TestXchg.cpp
               TestXlat.cpp)
target_link_libraries(unittest 8086sim gtest gmock)

add_test(unittest ./unittest --gtest_color=yes)

add_subdirectory(programs)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)

macro(system_test test_file)
    configure_file(${test_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    add_test(${test_file} python ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    set_property(TEST ${test_file} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_BINARY_DIR}/../python")
endmacro(system_test)

system_test(moves.py)
system_test(calls_and_jumps.py)
