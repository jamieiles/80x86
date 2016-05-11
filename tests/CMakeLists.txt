include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim)

add_executable(unittest
               main.cpp
               Arithmetic.cpp
               TestAdc.cpp
               TestAdd.cpp
               TestFifo.cpp
               TestFlags.cpp
               TestIncDec.cpp
               TestIO.cpp
               TestMemory.cpp
               TestModRM.cpp
               TestMov.cpp
               TestPointer.cpp
               TestPush.cpp
               TestRegisterFile.cpp
               TestSub.cpp
               TestXchg.cpp
               TestXlat.cpp)
target_link_libraries(unittest 8086sim gtest gmock)

add_test(unittest ./unittest --gtest_color=yes)
