include(Verilator)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_TTF_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(cppmodel)
add_subdirectory(RTLCPU)
add_subdirectory(display)
include_directories(cppmodel
                    display
                    common
                    RTLCPU
                    ../tests/rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/RTLCPU)

add_executable(simulator
               Simulator.cpp)
target_link_libraries(simulator
                      simcommon
                      8086sim
                      simdisplay)
nasm_file(bios.asm bios.bin)

install(TARGETS simulator
        COMPONENT simulator
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        RESOURCE DESTINATION share/s80x86)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bios.bin
        DESTINATION share/s80x86
        COMPONENT simulator)
