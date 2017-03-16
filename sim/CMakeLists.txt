find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_TTF_INCLUDE_DIR})

add_subdirectory(cpu)
add_subdirectory(display)
include_directories(cpu display)

add_executable(simulator
               Simulator.cpp)
target_link_libraries(simulator
                      8086sim
                      simdisplay)
nasm_file(bios.asm bios.bin)

install(TARGETS simulator
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        RESOURCE DESTINATION share/s80x86)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bios.bin
        DESTINATION share/s80x86)
