include(Nasm)

nasm_file(OUTPUT moves.bin
          SOURCES moves.asm)
add_custom_target(tests_moves.asm ALL DEPENDS moves.bin)

nasm_file(OUTPUT calls_and_jumps.bin
          SOURCES calls_and_jumps.asm)
add_custom_target(tests_calls_and_jumps.asm ALL DEPENDS calls_and_jumps.bin)

nasm_file(OUTPUT modrm_override.bin
          SOURCES modrm_override.asm)
add_custom_target(tests_modrm_override.asm ALL DEPENDS modrm_override.bin)

nasm_file(OUTPUT irq.bin
          SOURCES irq.asm)
add_custom_target(tests_irq.asm ALL DEPENDS irq.bin)

nasm_file(OUTPUT singlestep.bin
          SOURCES singlestep.asm)
add_custom_target(tests_singlestep.asm ALL DEPENDS singlestep.bin)
