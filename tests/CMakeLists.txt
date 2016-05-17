include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim)

add_executable(unittest
               main.cpp
               Arithmetic.cpp
               TestAdc.cpp
               TestAdd.cpp
               TestAscii.cpp
               TestCmp.cpp
               TestExtend.cpp
               TestFifo.cpp
               TestFlags.cpp
               TestIncDec.cpp
               TestInt.cpp
               TestIO.cpp
               TestMemory.cpp
               TestModRM.cpp
               TestMov.cpp
               TestMul.cpp
               TestNeg.cpp
               TestPointer.cpp
               TestPush.cpp
               TestRegisterFile.cpp
               TestSbb.cpp
               TestSub.cpp
               TestTrap.cpp
               TestXchg.cpp
               TestXlat.cpp)
target_link_libraries(unittest 8086sim gtest gmock)

add_test(unittest ./unittest --gtest_color=yes)

add_subdirectory(programs)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)
configure_file(moves.py ${CMAKE_CURRENT_BINARY_DIR}/moves.py)

add_test(moves python ${CMAKE_CURRENT_BINARY_DIR}/moves.py)
set_property(TEST moves PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_BINARY_DIR}/../python")
