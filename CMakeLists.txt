cmake_minimum_required(VERSION 3.20)
project(PentagonalXOR VERSION 0.1.0 LANGUAGES CXX)

add_executable(pentagonal_xor pentagonal_xor_cpp20.cpp)
target_compile_features(pentagonal_xor PRIVATE cxx_std_20)
set_target_properties(pentagonal_xor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(MSVC)
    target_compile_options(pentagonal_xor PRIVATE /W4 /permissive-)
else()
    target_compile_options(pentagonal_xor PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_test(NAME parity_example
        COMMAND pentagonal_xor 0.80 0.70 0.90 0.60 0.95)
    set_tests_properties(parity_example PROPERTIES
        PASS_REGULAR_EXPRESSION "Recursive XOR : 51\\.72800000%")

    add_test(NAME parity_all_zero COMMAND pentagonal_xor 0 0 0 0 0)
    set_tests_properties(parity_all_zero PROPERTIES
        PASS_REGULAR_EXPRESSION "Recursive XOR : 0\\.00000000%")

    add_test(NAME parity_odd COMMAND pentagonal_xor 1 1 1 0 0)
    set_tests_properties(parity_odd PROPERTIES
        PASS_REGULAR_EXPRESSION "Recursive XOR : 100\\.00000000%")

    add_test(NAME invalid_feature COMMAND pentagonal_xor 0.8 0.7 1.2 0.6 0.95)
    set_tests_properties(invalid_feature PROPERTIES WILL_FAIL TRUE)
endif()

install(TARGETS pentagonal_xor RUNTIME DESTINATION bin)
install(FILES README.md LICENSE DESTINATION share/doc/pentagonal-xor)
