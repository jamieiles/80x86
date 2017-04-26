function(add_fpga)
    set(options "")
    set(oneValueArgs PROJECT FAMILY PART)
    set(multiValueArgs SOURCES DEPENDS)
    cmake_parse_arguments(add_fpga "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(source ${ALL_SOURCES})
        list(APPEND SOURCE_ARGS --source=${source})
    endforeach(source)

    add_custom_command(OUTPUT ${add_fpga_PROJECT}.qpf
                       COMMAND ${QUARTUS_SH_EXECUTABLE} --prepare -f ${add_fpga_FAMILY} -t ${add_fpga_PROJECT} ${add_fpga_PROJECT}
                       DEPENDS ${add_fpga_PROJECT}.qsf)
    add_custom_command(OUTPUT ${add_fpga_PROJECT}.map.rpt
                       COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/microcode.mif .
                       COMMAND ${QUARTUS_MAP_EXECUTABLE} ${SOURCE_ARGS} --family ${add_fpga_FAMILY} --optimize=speed ${add_fpga_PROJECT}
                       DEPENDS ${add_fpga_DEPENDS} ${add_fpga_SOURCES} ${add_fpga_PROJECT}.qpf ${add_fpga_PROJECT}.qsf)
    add_custom_command(OUTPUT ${add_fpga_PROJECT}.fit.rpt
                       COMMAND ${QUARTUS_FIT_EXECUTABLE} --part=${add_fpga_PART} --read_settings_file=on --set=SDC_FILE=${CMAKE_CURRENT_SOURCE_DIR}/${add_fpga_PROJECT}.sdc ${add_fpga_PROJECT}
                       DEPENDS ${add_fpga_PROJECT}.map.rpt ${add_fpga_PROJECT}.sdc)
    add_custom_command(OUTPUT ${add_fpga_PROJECT}.asm.rpt
                       COMMAND ${QUARTUS_ASM_EXECUTABLE} ${add_fpga_PROJECT}
                       DEPENDS ${add_fpga_PROJECT}.fit.rpt)
    add_custom_command(OUTPUT ${add_fpga_PROJECT}.sta.rpt ${add_fpga_PROJECT}.sof
                       COMMAND ${QUARTUS_STA_EXECUTABLE} ${add_fpga_PROJECT}
                       DEPENDS ${add_fpga_PROJECT}.asm.rpt)
endfunction()
