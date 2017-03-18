file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

macro(configure_test test_file)
    configure_file(${test_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
endmacro(configure_test)

macro(system_test test_file sim)
    add_test(${sim}_${test_file} python ${CMAKE_CURRENT_BINARY_DIR}/${test_file} ${sim})
    set_tests_properties(${sim}_${test_file} PROPERTIES TIMEOUT 5)
    set_property(TEST ${sim}_${test_file} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/..:${CMAKE_CURRENT_BINARY_DIR}/../../python")
endmacro(system_test)

configure_test(moves.py)
configure_test(calls_and_jumps.py)
system_test(moves.py Sim)
system_test(moves.py RTLCPU)
system_test(calls_and_jumps.py Sim)
