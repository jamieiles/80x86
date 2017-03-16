add_library(8086sim SHARED
            RegisterFile.h
            RegisterFile.cpp
            Memory.h
            Memory.cpp
            ModRM.h
            ModRM.cpp
            Emulate.h
            Emulate.cpp)
install(TARGETS 8086sim
        LIBRARY DESTINATION lib)
