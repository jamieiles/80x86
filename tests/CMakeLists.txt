include_directories(../googletest/googletest/include)
include_directories(../sim)

add_executable(unittest
               main.cpp
               TestFifo.cpp
               TestRegisterFile.cpp
               TestMemory.cpp)
target_link_libraries(unittest 8086sim gtest)

add_test(unittest ./unittest)
