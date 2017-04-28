set(CMAKE_SYSTEM_NAME none)
set(CMAKE_SYSTEM_PROCESSOR ia16)
set(CMAKE_C_COMPILER ia16-elf-gcc)
set(CMAKE_ASM_COMPILER ia16-elf-gcc)

# IA16 GCC doesn't support -g
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_ASM_FLAGS_DEBUG "")

set(CMAKE_C_FLAGS "-Wall -Werror -fstack-usage -Wstack-usage=128 -fconserve-stack -ffreestanding -O2 -std=gnu99 -nostdinc -mtune=i8086 -march=i8086")
set(CMAKE_ASM_FLAGS "-Wall -Werror -ffreestanding -O2 -std=gnu99 -nostdinc -mtune=i8086 -march=i8086")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-ffreestanding -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/bios.x")

add_executable(bios
               bios.c
	       disk.c
               entry.S
               io.h
	       keyboard.c
               sd.c
               sd.h
	       serial.c
	       serial.h)