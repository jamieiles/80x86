include_directories(../common)

add_library(8086sim SHARED
            ModRM.h
            ModRM.cpp
            Emulate.h
            Emulate.cpp
            SoftwareCPU.h)
target_link_libraries(8086sim simcommon)
install(TARGETS 8086sim
        LIBRARY DESTINATION lib
        COMPONENT simulator)
