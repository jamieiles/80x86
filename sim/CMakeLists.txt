add_library(8086sim SHARED
            Fifo.h
            RegisterFile.h
            RegisterFile.cpp
            Memory.h
            Memory.cpp
            ModRM.h
            ModRM.cpp
            Emulate.h
            Emulate.cpp)
