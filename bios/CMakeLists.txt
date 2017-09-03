include(GetGitRevisionDescription)

set(CMAKE_SYSTEM_NAME none)
set(CMAKE_SYSTEM_PROCESSOR ia16)
set(CMAKE_C_COMPILER ia16-elf-gcc)
set(CMAKE_ASM_COMPILER ia16-elf-gcc)

# IA16 GCC doesn't support -g
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_ASM_FLAGS_DEBUG "")

set(CMAKE_C_FLAGS "-Wall -Werror -ffreestanding -Os -std=gnu99 -nostdinc -mtune=i80186 -march=i80186")
set(CMAKE_ASM_FLAGS "-Wall -Werror -ffreestanding -Os -std=gnu99 -nostdinc -mtune=i80186 -march=i80186")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/bios.map -ffreestanding -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/bios.x")

get_git_head_revision(BIOS_NAME BIOS_BUILD HEAD)
git_local_changes(LOCAL_CHANGES)
string(TIMESTAMP BIOS_BUILD_DATE "%m/%d/%y")

add_definitions(-D__PLATFORM__="${BIOS_PLATFORM}"
                -D__BUILD__="${BIOS_BUILD}+${LOCAL_CHANGES}"
                -DBIOS_BUILD_DATE=\"${BIOS_BUILD_DATE}\")

add_executable(bios-${BIOS_PLATFORM}
               bios.c
	       disk.c
	       display.c
               entry.S
               io.h
	       keyboard.c
               sd.c
               sd.h
	       serial.c
	       serial.h
	       timer.h
	       timer.c
               utils.h
               utils.c)
