include(Verilator)

set_source_files_properties(${VERILATED_DPI_CPP} PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter -Wno-error")
add_library(verilator STATIC ${VERILATOR_LIB_SOURCES})

verilate(Fifo ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v)
verilate(Prefetch ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v)
verilate(RegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v)
verilate(SegmentRegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/SegmentRegisterFile.v)
verilate(ModRMDecode ${CMAKE_CURRENT_SOURCE_DIR}/ModRMDecode.v)
verilate(ImmediateReader ${CMAKE_CURRENT_SOURCE_DIR}/ImmediateReader.v)
verilate(LoadStore ${CMAKE_CURRENT_SOURCE_DIR}/LoadStore.v)
