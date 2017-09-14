include(Quartus)

option(DE0_CV_SERIAL_STDIO "Use the serial port for stdio" OFF)

get_property(ALU_SOURCES GLOBAL PROPERTY G_ALU_SOURCES)
get_property(CORE_SOURCES GLOBAL PROPERTY G_CORE_SOURCES)

configure_file(Top.qsf Top.qsf)
configure_file(Top.srf Top.srf)

if(${DE0_CV_SERIAL_STDIO})
add_definitions(-DSERIAL_STDIO)
endif()
set(BIOS_PLATFORM "de0-cv")
add_subdirectory(../../bios ${CMAKE_CURRENT_BINARY_DIR}/bios
                 EXCLUDE_FROM_ALL)

set(FPGA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/defines.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/SysPLL.v
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/Top.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/VirtualJTAG.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/JTAGBridge.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../bios/BIOS.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../bios/BIOSControlRegister.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../irq/IRQControl.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../timer/Timer.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../sdram/Counter.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../sdram/SDRAMController.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../sdram/SDRAMConfigRegister.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/VGAController.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/VGASync.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/FontColorLUT.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/FrameBuffer.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/FrameBufferRAM.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../VGA/VGARegisters.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/MemArbiter.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/BitSync.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/SyncPulse.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/cdc/MCP.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../uart/BaudRateGen.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../uart/Transmitter.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../uart/Receiver.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../uart/Uart.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../uart/UartPorts.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../ps2/PS2Host.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../ps2/PS2Controller.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../spi/SPIMaster.sv
    ${CMAKE_CURRENT_SOURCE_DIR}/../spi/SPIPorts.sv)

set(ALL_SOURCES ${CORE_SOURCES} ${FPGA_SOURCES})

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/Microcode.v PROPERTIES
                            COMPILE_FLAGS "--verilog_macro=\"MICROCODE_ROM_PATH=\\\\\\\"${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/\\\\\\\"")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/../VGA/FontColorLUT.sv PROPERTIES
                            COMPILE_FLAGS "--verilog_macro=\"FONT_ROM_PATH=\\\\\\\"${CMAKE_CURRENT_BINARY_DIR}\\\\\\\"\"")

add_custom_command(OUTPUT microcode.mif
                   COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/microcode.mif .
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/microcode.mif generate_microcode)
add_custom_command(OUTPUT bios.mif
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/make-bios-mif ${CMAKE_CURRENT_BINARY_DIR}/bios/bios-de0-cv bios.mif
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bios/bios-de0-cv)
add_custom_command(OUTPUT font.bin
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/mkfontrom ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/fonts/cp437-8x8 font.bin
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/fonts/cp437-8x8)
add_fpga(PROJECT Top
         FAMILY "Cyclone V"
         PART "5CEBA4F23C7"
         SOURCES ${ALL_SOURCES}
         DEPENDS microcode.mif bios.mif font.bin)

add_custom_command(OUTPUT __program_de0_cv__
                   COMMAND ${QUARTUS_PGM_EXECUTABLE} --mode=jtag -o \"P\;Top.sof\"
                   DEPENDS Top.sof)

add_custom_target(de0-cv DEPENDS Top.sta.rpt)
add_custom_target(de0-cv-program DEPENDS __program_de0_cv__)
