include(Nasm)

nasm_file(OUTPUT de0-cv-bios.bin
          SOURCES bios.asm
          DEPENDS
          debug.asm
          disk.asm
          io.asm
          keyboard.asm
          sdcard.asm
          video.asm)
add_custom_target(de0-cv-bios ALL DEPENDS de0-cv-bios.bin)
