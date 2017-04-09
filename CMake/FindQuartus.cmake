include(FindPackageHandleStandardArgs)

set(QUARTUS_PATHS
    /opt/altera/quartus/bin/
    /opt/altera/14.1/quartus/bin/
    ${HOME}/altera/14.1/quartus/bin)

find_program(QUARTUS_SH_EXECUTABLE NAMES quartus_sh
             PATHS ${QUARTUS_PATHS})
find_program(QUARTUS_MAP_EXECUTABLE NAMES quartus_map
             PATHS ${QUARTUS_PATHS})
find_program(QUARTUS_FIT_EXECUTABLE NAMES quartus_fit
             PATHS ${QUARTUS_PATHS})
find_program(QUARTUS_ASM_EXECUTABLE NAMES quartus_asm
             PATHS ${QUARTUS_PATHS})
find_program(QUARTUS_STA_EXECUTABLE NAMES quartus_sta
             PATHS ${QUARTUS_PATHS})
find_program(QUARTUS_PGM_EXECUTABLE NAMES quartus_pgm
             PATHS ${QUARTUS_PATHS})

find_package_handle_standard_args(Quartus FOUND_VAR QUARTUS_FOUND
                                  REQUIRED_VARS
                                  QUARTUS_SH_EXECUTABLE
                                  QUARTUS_MAP_EXECUTABLE
                                  QUARTUS_FIT_EXECUTABLE
                                  QUARTUS_ASM_EXECUTABLE
                                  QUARTUS_STA_EXECUTABLE
                                  QUARTUS_PGM_EXECUTABLE)
