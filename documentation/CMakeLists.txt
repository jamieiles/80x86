
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf
                   COMMAND asciidoctor-pdf ${CMAKE_CURRENT_SOURCE_DIR}/development-guide.asciidoc -a pdf-style=${CMAKE_CURRENT_SOURCE_DIR}/theme.yml -a toc -o ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf --doctype=book
                   DEPENDS development-guide.asciidoc theme.yml)
add_custom_target(documentation ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/development-guide.pdf)
