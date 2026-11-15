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
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 51\\.72800000%")

    add_test(NAME parity_all_zero COMMAND pentagonal_xor 0 0 0 0 0)
    set_tests_properties(parity_all_zero PROPERTIES
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 0\\.00000000%")

    add_test(NAME parity_odd COMMAND pentagonal_xor 1 1 1 0 0)
    set_tests_properties(parity_odd PROPERTIES
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 100\\.00000000%")

    add_test(NAME invalid_feature COMMAND pentagonal_xor 0.8 0.7 1.2 0.6 0.95)
    set_tests_properties(invalid_feature PROPERTIES WILL_FAIL TRUE)

    add_test(NAME invalid_nan COMMAND pentagonal_xor 0.8 0.7 nan 0.6 0.95)
    set_tests_properties(invalid_nan PROPERTIES WILL_FAIL TRUE)

    add_test(NAME invalid_arity COMMAND pentagonal_xor 0.8 0.7 0.9 0.6)
    set_tests_properties(invalid_arity PROPERTIES WILL_FAIL TRUE)

    add_executable(parity_reference_test tests/parity_reference_test.cpp)
    target_compile_features(parity_reference_test PRIVATE cxx_std_20)
    set_target_properties(parity_reference_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME parity_reference_exhaustive COMMAND parity_reference_test)

    add_executable(deca_sigma_test tests/deca_sigma_test.cpp)
    target_compile_features(deca_sigma_test PRIVATE cxx_std_20)
    set_target_properties(deca_sigma_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME deca_sigma_roundtrip COMMAND deca_sigma_test)

    add_executable(tri_pi_test tests/tri_pi_test.cpp)
    target_compile_features(tri_pi_test PRIVATE cxx_std_20)
    set_target_properties(tri_pi_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME tri_pi_three_routes COMMAND tri_pi_test)

    add_executable(coreao_test tests/coreao_test.cpp)
    target_compile_features(coreao_test PRIVATE cxx_std_20)
    set_target_properties(coreao_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME coreao_unified_scalar COMMAND coreao_test)

    if(MSVC)
        target_compile_options(parity_reference_test PRIVATE /W4 /permissive-)
        target_compile_options(deca_sigma_test PRIVATE /W4 /permissive-)
        target_compile_options(tri_pi_test PRIVATE /W4 /permissive-)
        target_compile_options(coreao_test PRIVATE /W4 /permissive-)
    else()
        target_compile_options(parity_reference_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(deca_sigma_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(tri_pi_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreao_test PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

install(TARGETS pentagonal_xor RUNTIME DESTINATION bin)
install(FILES README.md LICENSE docs/DECASIGMA.md docs/TRI_PI.md docs/COREAO.md DESTINATION share/doc/pentagonal-xor)
