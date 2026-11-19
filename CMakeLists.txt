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


# Separate CLI: old pentagonal_xor executable stays unchanged.
add_executable(pentagonal_control tools/pentagonal_control.cpp)
target_compile_features(pentagonal_control PRIVATE cxx_std_20)
install(TARGETS pentagonal_control RUNTIME DESTINATION bin)
if(BUILD_TESTING)
  add_test(NAME pentagonal_control_json COMMAND pentagonal_control 0x3fffffff 0x3fffffff or)
  set_tests_properties(pentagonal_control_json PROPERTIES
    PASS_REGULAR_EXPRESSION "logical_evaluations")
  add_test(NAME pentagonal_depth_three_benchmark COMMAND pentagonal_gates_benchmark)
  set_tests_properties(pentagonal_depth_three_benchmark PROPERTIES
    PASS_REGULAR_EXPRESSION "Interpretation")
endif()


# Supplier-side GAMP 4-style verification, separate from regulated PQ/QA.
if(BUILD_TESTING)
  add_executable(pentagonal_gamp4_validation_test
                 tests/pentagonal_gamp4_validation_test.cpp)
  target_compile_features(pentagonal_gamp4_validation_test PRIVATE cxx_std_20)
  add_test(NAME gamp4_pentagonal_urs_11_to_22
           COMMAND pentagonal_gamp4_validation_test)
endif()
install(FILES docs/PENTAGONAL_GAMP4_VALIDATION.md
        DESTINATION share/doc/pentagonal-xor)


# LIGAZE: real native C++20 executable distinct from the Java JAR.
add_executable(pentagonal_ligaze tools/pentagonal_ligaze.cpp)
target_compile_features(pentagonal_ligaze PRIVATE cxx_std_20)
install(TARGETS pentagonal_ligaze RUNTIME DESTINATION bin)
install(FILES shared/pentagonal_ligaze.hpp DESTINATION include/swe)
if(BUILD_TESTING)
  add_executable(pentagonal_ligaze_test tests/pentagonal_ligaze_test.cpp)
  target_compile_features(pentagonal_ligaze_test PRIVATE cxx_std_20)
  add_test(NAME pentagonal_ligaze_exhaustive_cor_witness COMMAND pentagonal_ligaze_test)
  add_test(NAME pentagonal_ligaze_native_cli COMMAND pentagonal_ligaze 0x15 0x15 0x1f 0x1f)
  set_tests_properties(pentagonal_ligaze_native_cli PROPERTIES
    PASS_REGULAR_EXPRESSION "native-cxx20")
endif()
install(FILES docs/PENTAGONAL_LIGAZE_NATIVE.md DESTINATION share/doc/pentagonal-xor)


# Platform-matrix LIGAZE COR workload: independent native C++20 benchmark.
add_executable(pentagonal_ligaze_benchmark benchmarks/pentagonal_ligaze_benchmark.cpp)
target_compile_features(pentagonal_ligaze_benchmark PRIVATE cxx_std_20)
if(BUILD_TESTING)
  add_test(NAME pentagonal_ligaze_benchmark_smoke
    COMMAND pentagonal_ligaze_benchmark 1000 100 1)
  set_tests_properties(pentagonal_ligaze_benchmark_smoke PROPERTIES
    PASS_REGULAR_EXPRESSION "SWE-LIGAZE-BENCH-v1")
endif()


# Explicit, opt-in Windows LIGAZE benchmark AB variants.
# Isolated to benchmark target; no default shipping or Win32 API replacement.
set(SWE_LIGAZE_WINDOWS_AB_VARIANT "promoted" CACHE STRING
    "Windows LIGAZE benchmark experiment: promoted|baseline|safe_header|full_header|ltcg|safe_header_ltcg|platform_v2|platform_v2_ltcg|full_header_v2")
set_property(CACHE SWE_LIGAZE_WINDOWS_AB_VARIANT PROPERTY STRINGS
    promoted baseline safe_header full_header ltcg safe_header_ltcg platform_v2 platform_v2_ltcg full_header_v2)
if(MSVC)
  # Actual measured optimization: /GL + /LTCG, enabled in Release only.
  # The default benchmark is promoted; explicit baseline stays unoptimized
  # so later AB runs remain comparable against a real pre-promotion control.
  set_property(TARGET pentagonal_ligaze PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
  if(SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "safe_header" OR
     SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "safe_header_ltcg")
    target_compile_definitions(pentagonal_ligaze_benchmark PRIVATE
      SWE_LIGAZE_WINDOWS_SAFE_HEADER_EXPERIMENT=1)
  elseif(SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "full_header")
    target_compile_definitions(pentagonal_ligaze_benchmark PRIVATE
      SWE_LIGAZE_WINDOWS_FULL_HEADER_EXPERIMENT=1)
  elseif(SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "platform_v2" OR
         SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "platform_v2_ltcg")
    target_compile_definitions(pentagonal_ligaze_benchmark PRIVATE
      SWE_LIGAZE_WINDOWS_PLATFORM_V2_EXPERIMENT=1)
  elseif(SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "full_header_v2")
    target_compile_definitions(pentagonal_ligaze_benchmark PRIVATE
      SWE_LIGAZE_WINDOWS_FULL_HEADER_V2_EXPERIMENT=1)
  elseif(NOT SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "baseline" AND
         NOT SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "promoted" AND
         NOT SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "ltcg")
    message(FATAL_ERROR "Unknown Windows AB experiment variant: ${SWE_LIGAZE_WINDOWS_AB_VARIANT}")
  endif()
  if(SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "promoted" OR
     SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "ltcg" OR
     SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "safe_header_ltcg" OR
     SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "platform_v2_ltcg")
    # MSVC Release already enables /O2; test additional /GL and /LTCG.
    set_property(TARGET pentagonal_ligaze_benchmark PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
else()
  if(NOT SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "baseline" AND
     NOT SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "promoted")
    message(FATAL_ERROR "Windows-specific AB variant selected on non-MSVC compiler")
  endif()
endif()

# Version-2 safe platform metadata and API integrity, including negative
# compile-only test of the user's do-nothing Win32 API replacement header.
add_executable(pentagonal_platform_v2_contract_test tests/pentagonal_platform_v2_contract.cpp)
target_compile_features(pentagonal_platform_v2_contract_test PRIVATE cxx_std_20)
if(MSVC AND SWE_LIGAZE_WINDOWS_AB_VARIANT STREQUAL "full_header_v2")
  target_compile_definitions(pentagonal_platform_v2_contract_test PRIVATE
    SWE_LIGAZE_WINDOWS_FULL_HEADER_V2_EXPERIMENT=1)
endif()
if(BUILD_TESTING)
  add_test(NAME pentagonal_ligaze_platform_v2_api_contract
    COMMAND pentagonal_platform_v2_contract_test)
endif()

# Native PowerShell release-gate contract; tested wherever PowerShell 7 exists.
if(BUILD_TESTING)
  find_program(SWE_PWSH7 NAMES pwsh)
  if(SWE_PWSH7)
    add_test(NAME release_safety_negative_fixture_contract
      COMMAND "${SWE_PWSH7}" -NoLogo -NoProfile -File
        "${CMAKE_CURRENT_SOURCE_DIR}/ci/Test-ReleaseSafetyPolicy.ps1")
  endif()
endif()


# Experimental 9x9 VISTA audit extension.
# Keeps original six-gate Machine and its canonical XOR implementation unchanged.
add_executable(pentagonal_vista9x9 tools/pentagonal_vista9x9.cpp)
target_compile_features(pentagonal_vista9x9 PRIVATE cxx_std_20)
install(TARGETS pentagonal_vista9x9 RUNTIME DESTINATION bin)
install(FILES shared/pentagonal_vista_9x9.hpp DESTINATION include/swe)
install(FILES docs/VISTA_9X9.md docs/VISTA_9X9_SHOWCASE.md
              docs/VISTA_9X9_LADDER.svg
        DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(pentagonal_vista_9x9_test tests/pentagonal_vista_9x9_test.cpp)
  target_compile_features(pentagonal_vista_9x9_test PRIVATE cxx_std_20)
  add_test(NAME pentagonal_vista_9x9_complete_and_readonly
           COMMAND pentagonal_vista_9x9_test)
  add_test(NAME pentagonal_vista_9x9_native_demo
           COMMAND pentagonal_vista9x9 0x17 0x1f)
  set_tests_properties(pentagonal_vista_9x9_native_demo PROPERTIES
    PASS_REGULAR_EXPRESSION "total=81")
endif()


# SIGMA-PENTA-PING v1 and SigmaDi / HANDecaSigma[11] OSI-6 display.
# Exploratory derived measurements; canonical six-stage Machine stays intact.
add_library(swe_sigma_penta_ping INTERFACE)
target_compile_features(swe_sigma_penta_ping INTERFACE cxx_std_20)
target_include_directories(swe_sigma_penta_ping INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/shared>
  $<INSTALL_INTERFACE:include>)
install(TARGETS swe_sigma_penta_ping EXPORT swe_sigma_penta_ping_targets)
install(EXPORT swe_sigma_penta_ping_targets
  FILE SWESigmaPentaPingTargets.cmake
  NAMESPACE swe::
  DESTINATION lib/cmake/SWESigmaPentaPing)
install(FILES
  shared/sigma_penta_ping.hpp
  shared/sigmadi_handecasigma_presentation.hpp
  DESTINATION include/swe)
install(FILES
  docs/SIGMA_PENTA_HAN_PRESENTATION.md
  docs/SIGMA_PENTA_HAN_VISTA_BOARD.svg
  DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(sigma_penta_ping_test tests/sigma_penta_ping_test.cpp)
  target_compile_features(sigma_penta_ping_test PRIVATE cxx_std_20)
  add_test(NAME sigma_penta_ping_triple_ring_and_braid
           COMMAND sigma_penta_ping_test)
  add_executable(sigmadi_handecasigma_presentation_test
    tests/sigmadi_handecasigma_presentation_test.cpp)
  target_compile_features(sigmadi_handecasigma_presentation_test PRIVATE cxx_std_20)
  add_test(NAME sigma_di_han_osi6_audit
           COMMAND sigmadi_handecasigma_presentation_test)
endif()
