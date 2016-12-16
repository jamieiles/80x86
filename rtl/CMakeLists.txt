include(Verilator)

verilate(Fifo ${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v)
verilate(Prefetch ${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v)
verilate(RegisterFile ${CMAKE_CURRENT_SOURCE_DIR}/RegisterFile.v)
