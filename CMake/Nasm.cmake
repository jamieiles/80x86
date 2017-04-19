function(nasm_file)
    set(options "")
    set(oneValueArgs OUTPUT)
    set(multiValueArgs SOURCES DEPENDS)
    cmake_parse_arguments(nasm_file "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(source ${nasm_file_SOURCES})
        list(APPEND SOURCE_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    endforeach(source)

    add_custom_command(OUTPUT ${nasm_file_OUTPUT}
                       COMMAND nasm -i${CMAKE_CURRENT_SOURCE_DIR}/
                           -o ${nasm_file_OUTPUT} -f bin ${SOURCE_ARGS}
                       DEPENDS ${nasm_file_DEPENDS} ${nasm_file_SOURCES})
endfunction()
