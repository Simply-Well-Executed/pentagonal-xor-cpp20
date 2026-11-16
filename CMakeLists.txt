cmake_minimum_required(VERSION 3.20)
project(PentagonalXOR VERSION 0.2.0 LANGUAGES CXX)

# Produce usable debug symbols even in optimized Release builds.
# MSVC PDBs are distributable outputs of this project, not the compiler itself.
if(MSVC)
    add_compile_options(/Zi)
    add_link_options(/DEBUG:FULL)
else()
    add_compile_options(-g)
endif()

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

    add_executable(coreao_gain_test tests/coreao_gain_test.cpp)
    target_compile_features(coreao_gain_test PRIVATE cxx_std_20)
    set_target_properties(coreao_gain_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME coreao_rf_9dbi_eirp COMMAND coreao_gain_test)

    add_executable(counterarccosine_secondwave_test
        tests/counterarccosine_secondwave_test.cpp)
    target_compile_features(counterarccosine_secondwave_test PRIVATE cxx_std_20)
    add_test(NAME counterarccosine_secondwave_phase
        COMMAND counterarccosine_secondwave_test)

    add_executable(stability10d_test tests/stability10d_test.cpp)
    target_compile_features(stability10d_test PRIVATE cxx_std_20)
    add_test(NAME stability10d_hexabyte_and_minterms COMMAND stability10d_test)

    add_executable(coreo_stereo_test tests/coreo_stereo_test.cpp)
    target_compile_features(coreo_stereo_test PRIVATE cxx_std_20)
    add_test(NAME coreo_stereo_invertibility COMMAND coreo_stereo_test)

    if(MSVC)
        target_compile_options(parity_reference_test PRIVATE /W4 /permissive-)
        target_compile_options(deca_sigma_test PRIVATE /W4 /permissive-)
        target_compile_options(tri_pi_test PRIVATE /W4 /permissive-)
        target_compile_options(coreao_test PRIVATE /W4 /permissive-)
        target_compile_options(coreao_gain_test PRIVATE /W4 /permissive-)
        target_compile_options(counterarccosine_secondwave_test PRIVATE /W4 /permissive-)
        target_compile_options(stability10d_test PRIVATE /W4 /permissive-)
        target_compile_options(coreo_stereo_test PRIVATE /W4 /permissive-)
    else()
        target_compile_options(parity_reference_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(deca_sigma_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(tri_pi_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreao_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreao_gain_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(counterarccosine_secondwave_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(stability10d_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreo_stereo_test PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Ship the usable, header-only audit APIs rather than keeping them only
# as test dependencies. No run-time mode switches or alternate XOR paths.
add_library(swe_audit_headers INTERFACE)
target_compile_features(swe_audit_headers INTERFACE cxx_std_20)
target_include_directories(swe_audit_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shared>
    $<INSTALL_INTERFACE:include>)
install(TARGETS swe_audit_headers EXPORT swe_audit_headers_targets)
install(EXPORT swe_audit_headers_targets
    FILE SWEAuditHeadersTargets.cmake
    NAMESPACE swe::
    DESTINATION lib/cmake/SWEAuditHeaders)
install(FILES
    shared/deca_sigma.hpp
    shared/tri_pi.hpp
    shared/coreao.hpp
    shared/coreao_gain.hpp
    shared/stability10d.hpp
    shared/coreo_stereo.hpp
    shared/pentagonal_xor_std_free.h
    DESTINATION include/swe)

install(TARGETS pentagonal_xor RUNTIME DESTINATION bin)
if(MSVC)
    install(FILES "$<TARGET_PDB_FILE:pentagonal_xor>" DESTINATION symbols OPTIONAL)
endif()
install(FILES README.md LICENSE docs/DECASIGMA.md docs/TRI_PI.md docs/COREAO.md docs/BUILD_EVIDENCE.md docs/RELEASE_PREFLIGHT.md docs/STABILITY10D.md docs/ANDROID.md docs/COREAO_GAIN.md docs/COUNTERARCCOSINE_2NDWAVE.svg DESTINATION share/doc/pentagonal-xor)


# Experimental gate controller, isolated from the original XOR executable.
if(BUILD_TESTING)
  add_executable(pentagonal_gates_test tests/pentagonal_gates_test.cpp)
  target_compile_features(pentagonal_gates_test PRIVATE cxx_std_20)
  add_test(NAME pentagonal_gates_truth_witness_and_flop COMMAND pentagonal_gates_test)
  add_executable(pentagonal_gates_benchmark tests/pentagonal_gates_benchmark.cpp)
  target_compile_features(pentagonal_gates_benchmark PRIVATE cxx_std_20)
endif()
install(FILES shared/pentagonal_gates.hpp DESTINATION include/swe)
install(FILES docs/PENTAGONAL_GATE_CASCADE.md DESTINATION share/doc/pentagonal-xor)
