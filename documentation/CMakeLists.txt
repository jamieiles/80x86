configure_file(development-guide.asciidoc development-guide.asciidoc)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/instructions.asciidoc
                   COMMAND PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../python ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/gen-instructions > ${CMAKE_CURRENT_BINARY_DIR}/instructions.asciidoc
                   DEPENDS Cpu ${CMAKE_CURRENT_SOURCE_DIR}/instructions.templ ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/gen-instructions instructions.yaml)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf
                   COMMAND asciidoctor-pdf ${CMAKE_CURRENT_BINARY_DIR}/development-guide.asciidoc -a pdf-style=${CMAKE_CURRENT_SOURCE_DIR}/theme.yml -a toc -o ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf --doctype=book
                   DEPENDS development-guide.asciidoc theme.yml ${CMAKE_CURRENT_BINARY_DIR}/instructions.asciidoc)
add_custom_target(documentation ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf
        DESTINATION share/doc/s80x86
        COMPONENT simulator)
