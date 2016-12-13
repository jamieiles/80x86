include(Verilator)

verilate(${CMAKE_CURRENT_SOURCE_DIR}/Fifo.v Fifo)
verilate(${CMAKE_CURRENT_SOURCE_DIR}/Prefetch.v Prefetch)
