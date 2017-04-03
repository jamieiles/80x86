include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(../sim/common)
include_directories(../sim/cppmodel)
include_directories(../sim)
include_directories(include)

add_subdirectory(programs)
add_subdirectory(instructions)
add_subdirectory(integration)
add_subdirectory(simulator)
add_subdirectory(rtl)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)
