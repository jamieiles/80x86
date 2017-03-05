macro(system_test test_file)
    configure_file(${test_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    add_test(${test_file} python ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
    set_tests_properties(${test_file} PROPERTIES TIMEOUT 5)
    set_property(TEST ${test_file} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/..:${CMAKE_CURRENT_BINARY_DIR}/../../python")
endmacro(system_test)

system_test(moves.py)
system_test(calls_and_jumps.py)
