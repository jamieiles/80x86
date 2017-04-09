get_property(ALU_SOURCES GLOBAL PROPERTY G_ALU_SOURCES)
get_property(CORE_SOURCES GLOBAL PROPERTY G_CORE_SOURCES)

configure_file(Top.qsf Top.qsf)

set(FPGA_FAMILY "Cyclone IV E")
set(FPGA_PART "EP4CE22F17C6")

set(FPGA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Top.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/SysPLL.v
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/VirtualJTAG.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/JTAGBridge.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/BitSync.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/SyncPulse.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/MCP.sv)

set(ALL_SOURCES ${CORE_SOURCES} ${FPGA_SOURCES})

foreach(source ${ALL_SOURCES})
    list(APPEND SOURCE_ARGS --source=${source})
endforeach(source)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/Microcode.v PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/\\\"")

add_custom_command(OUTPUT Top.qpf
                   COMMAND ${QUARTUS_SH_EXECUTABLE} --prepare -f ${FPGA_FAMILY} -t Top Top
                   DEPENDS Top.qsf)
add_custom_command(OUTPUT Top.map.rpt
                   COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/microcode.mif .
                   COMMAND ${QUARTUS_MAP_EXECUTABLE} ${SOURCE_ARGS} --optimize=speed Top
                   DEPENDS ${CORE_SOURCES} ${FPGA_SOURCES} Top.qpf Top.qsf generate_microcode)
add_custom_command(OUTPUT Top.fit.rpt
                   COMMAND ${QUARTUS_FIT_EXECUTABLE} --part=${FPGA_PART} --read_settings_file=on --set=SDC_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Top.sdc Top
                   DEPENDS Top.map.rpt Top.sdc)
add_custom_command(OUTPUT Top.asm.rpt
                   COMMAND ${QUARTUS_ASM_EXECUTABLE} Top
                   DEPENDS Top.fit.rpt)
add_custom_command(OUTPUT Top.sta.rpt Top.sof
                   COMMAND ${QUARTUS_STA_EXECUTABLE} Top
                   DEPENDS Top.asm.rpt)
add_custom_command(OUTPUT __program_de0_nano__
                   COMMAND ${QUARTUS_PGM_EXECUTABLE} --mode=jtag -o \"P\;Top.sof\"
                   DEPENDS Top.sof)
add_custom_target(de0-nano DEPENDS Top.sta.rpt)
add_custom_target(de0-nano-program DEPENDS __program_de0_nano__)
