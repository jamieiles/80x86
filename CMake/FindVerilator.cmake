find_path(VERILATOR_INCLUDE_DIR verilated.h
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)
find_path(VERILATOR_VPI_INCLUDE_DIR vpi_user.h
          HINTS /usr/share/verilator/include/vltstd
          /usr/local/share/verilator/include/vltstd)
find_path(VERILATOR_VCD_INCLUDE_DIR verilated_vcd_c.h
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)
find_file(VERILATED_CPP verilated.cpp
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)
find_file(VERILATED_COV_CPP verilated_cov.cpp
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)
find_file(VERILATED_VCD_CPP verilated_vcd_c.cpp
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)
find_file(VERILATED_DPI_CPP verilated_dpi.cpp
          HINTS /usr/share/verilator/include
          /usr/local/share/verilator/include)

if (VERILATOR_INCLUDE_DIR AND EXISTS "${VERILATOR_INCLUDE_DIR}/verilated_config.h")
    file(STRINGS "${VERILATOR_INCLUDE_DIR}/verilated_config.h" VERILATOR_VERSION_STRING_LINE REGEX "^#define[ \t]+VERILATOR_VERSION[ \t]+\".*\"$")
    string(REGEX REPLACE "^#define[ \t]+VERILATOR_VERSION[ \t]+\"(.*)\"$" "\\1" VERILATOR_VERSION_STRING "${VERILATOR_VERSION_STRING_LINE}")
endif ()

set(VERILATOR_INCLUDE_DIRS ${VERILATOR_INCLUDE_DIR} ${VERILATOR_VPI_INCLUDE_DIR} ${VERILATOR_VCD_INCLUDE_DIR})
set(VERILATOR_LIB_SOURCES ${VERILATED_CPP} ${VERILATED_COV_CPP} ${VERILATED_VCD_CPP} ${VERILATED_DPI_CPP})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(VERILATOR
        REQUIRED_VARS VERILATOR_LIB_SOURCES VERILATOR_INCLUDE_DIRS
        VERSION_VAR VERILATOR_VERSION_STRING)
