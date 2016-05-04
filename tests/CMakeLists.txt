include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim)

add_executable(unittest
               main.cpp
               TestFifo.cpp
               TestRegisterFile.cpp
               TestMemory.cpp
               TestEmulate.cpp
               TestModRM.cpp)
target_link_libraries(unittest 8086sim gtest gmock)

add_test(unittest ./unittest --gtest_color=yes)
