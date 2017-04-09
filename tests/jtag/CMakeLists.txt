find_package(libusb-1.0 REQUIRED)

add_library(JTAGCPU SHARED
            JTAGCPU.cpp JTAGCPU.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag/jtag.c
            ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag/jtag-virtual.c)
include_directories(${LIBUSB_1_INCLUDE_DIRS})             
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag)
target_link_libraries(JTAGCPU ${LIBUSB_1_LIBRARIES})

add_executable(jtag-unittest
               $<TARGET_OBJECTS:instructions>
               main.cpp)
target_link_libraries(jtag-unittest
                      gtest
                      gmock
                      JTAGCPU)
