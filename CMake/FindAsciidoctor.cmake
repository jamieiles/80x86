include(FindPackageHandleStandardArgs)

find_program(ASCIIDOCTOR_EXECUTABLE NAMES asciidoctor-pdf
             PATHS /usr/bin /usr/local/bin)

find_package_handle_standard_args(Asciidoctor FOUND_VAR ASCIIDOCTOR_FOUND
                                  REQUIRED_VARS ASCIIDOCTOR_EXECUTABLE)
