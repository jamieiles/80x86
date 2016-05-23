find_program(NASM_PATH nasm
             DOC "Nasm assembler"
             PATHS /usr/bin/nasm)

macro(nasm_file filename output)
    add_custom_command(OUTPUT ${output}
                       COMMAND nasm -o ${output} -f bin ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                       DEPENDS ${filename})
    add_custom_target(tests_${filename} ALL
                      DEPENDS ${output})
endmacro(nasm_file)
