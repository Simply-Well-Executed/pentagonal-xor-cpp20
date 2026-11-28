cmake_minimum_required(VERSION 3.23)
# PR #32 current compiler policy: fail before project() for named Clang
# drivers and native Apple/OpenBSD hosts without an approved alternative.
# Compiler identity is checked again after detection to catch renamed binaries.
if(APPLE OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" OR
   CMAKE_HOST_SYSTEM_NAME STREQUAL "OpenBSD")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|native_apple_openbsd_no_approved_nonclang_compiler")
endif()
if((DEFINED CMAKE_CXX_COMPILER AND CMAKE_CXX_COMPILER MATCHES "[Cc][Ll][Aa][Nn][Gg]") OR
   (DEFINED ENV{CXX} AND "$ENV{CXX}" MATCHES "[Cc][Ll][Aa][Nn][Gg]"))
  message(FATAL_ERROR "TOOLCHAIN|HOLD|clang_compiler_prohibited")
endif()
project(PentagonalXOR VERSION 0.2.0 LANGUAGES CXX)
# PR #32 native MSVC-only (clang-cl is NOT MSVC).
if(NOT CMAKE_HOST_WIN32 OR NOT MSVC OR
   NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|native_MSVC_only")
endif()
# A renamed Clang/AppleClang driver must not bypass the path gate.
if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang)$")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|clang_family_compiler_prohibited")
endif()

# ISO C++20, strict O2 optimization (no O0/O3).
# Historical O3 evidence remains in Git history, never relabeled.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR APPLE)
  message(FATAL_ERROR "TOOLCHAIN|HOLD|no_approved_native_compiler_for_target")
endif()
foreach(_config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  if(MSVC)
    set(CMAKE_CXX_FLAGS_${_config} "/MD /O2 /DNDEBUG" CACHE STRING "MSVC optimized O2" FORCE)
  else()
    set(CMAKE_CXX_FLAGS_${_config} "-O2 -DNDEBUG" CACHE STRING "Native compiler optimized O2" FORCE)
  endif()
endforeach()

# Apply actionable diagnostics across ALL configurations and C++ targets.
if(MSVC)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/W4>
                      $<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall>
                      $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
                      $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>)
endif()


# Produce usable debug symbols even in optimized Release builds.
# MSVC PDBs are distributable outputs of this project, not the compiler itself.
if(MSVC)
    add_compile_options(/Zi)
    add_link_options(/DEBUG:FULL)
else()
    add_compile_options(-g)
endif()

# Cross-platform STD-free core packaging. The legacy application and
# experiments remain available with the explicit OFF setting. Core-only builds
# compile one ISO C++20 object and create a native static archive, without any
# STL source includes, C++ runtime link step, network fetch or executable tests.
option(SWE_STD_FREE_CORE_ONLY
  "Build native STD-free Suite plus canonical DecaSigma/TriPi/COREAO/stability headers" OFF)
if(SWE_STD_FREE_CORE_ONLY)
  add_library(swe_std_free_portable_core STATIC
    tests/std_free_portable_probe.cpp)
  target_include_directories(swe_std_free_portable_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/swe>)
  target_compile_features(swe_std_free_portable_core PUBLIC cxx_std_20)
  set_target_properties(swe_std_free_portable_core PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  install(TARGETS swe_std_free_portable_core ARCHIVE DESTINATION lib)
  install(FILES
    include/pxor_memory.hpp
    include/pentagonal_xor.hpp
    include/pentagonal_suite.hpp
    shared/pentagonal_xor_std_free.h
    shared/deca_sigma.hpp
    shared/tri_pi.hpp
    shared/euler_pi_inverse_square.hpp
    shared/sigma_form_geometry.hpp
    shared/sigma_form_area_q62.hpp
    shared/right_triangle_f123_original.hpp
    shared/f123_eighth_fold_experiment.hpp
    shared/sigma_e_redaxualchoice.hpp
    shared/sigma_f_redaxualchoice.hpp
    shared/four_pyramid_reduxualchoice.hpp
    shared/u_circle_fit.hpp
    shared/minza_topology_incidence.hpp
    shared/xdr_rdx_fold_tork.hpp
    shared/minza_direct_ping_bidi.hpp
    shared/minza_direct_ping_tridi.hpp
    shared/three_geometric_folds.hpp
    shared/rectangloid_primitive_cxx20.hpp
    shared/toolong_q62_core.hpp
    shared/slidingtoolong.hpp
    shared/eulers_jig_canonical.hpp
    shared/coreao.hpp
    shared/coreao_gain.hpp
    shared/coreo_stereo.hpp
    shared/stability10d.hpp
    shared/quantum_operand_lexicon.hpp
    shared/recovery.hpp
    shared/physicals_swimming.hpp
    shared/spherroid.hpp
    shared/physics_lab_geometry.hpp
    shared/physics_lab_formulium.hpp
    DESTINATION include/swe)
  message(STATUS "IRON native STD-free Suite and canonical scalar audits: archive-only, no legacy targets")
  return()
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

# Optional throughput library and separate benchmark: the established
# five-feature parity executable remains unmodified and retains its test contract.
add_library(swe_pentagonal_xor_throughput INTERFACE)
target_include_directories(swe_pentagonal_xor_throughput INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(swe_pentagonal_xor_throughput INTERFACE cxx_std_20)
# Independent STD-free five-feature CLI, using the canonical original
# parity function. The legacy STL-backed CLI remains for T-CROSS comparison.
if(WIN32 OR APPLE OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(pentagonal_xor_std_free src/parity_std_free_main.cpp)
  target_link_libraries(pentagonal_xor_std_free
    PRIVATE swe_pentagonal_xor_throughput)
  set_target_properties(pentagonal_xor_std_free PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  install(TARGETS pentagonal_xor_std_free RUNTIME DESTINATION bin)
endif()
# Native OS-backed stdout, monotonic time and memory mapping are
# implemented for Windows, Linux and supported macOS. Android uses the core-only archive.
if(WIN32 OR APPLE OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(pentagonal_xor_cli src/main.cpp)
  target_link_libraries(pentagonal_xor_cli PRIVATE swe_pentagonal_xor_throughput)
  set_target_properties(pentagonal_xor_cli PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  install(TARGETS pentagonal_xor_cli RUNTIME DESTINATION bin)
  add_executable(pentagonal_suite_cli src/suite_main.cpp)
  target_link_libraries(pentagonal_suite_cli PRIVATE swe_pentagonal_xor_throughput)
  set_target_properties(pentagonal_suite_cli PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  install(TARGETS pentagonal_suite_cli RUNTIME DESTINATION bin)
else()
  message(STATUS "STD-free throughput CLI: native I/O backend unsupported on this host")
endif()
# Both headers form the self-contained STD-free API; installing only
# pentagonal_xor.hpp would break downstream include resolution.
install(FILES
  include/pentagonal_xor.hpp
  include/pxor_memory.hpp
  include/pentagonal_suite.hpp
  DESTINATION include/swe)
install(FILES shared/quantum_operand_lexicon.hpp DESTINATION include/swe)

# Independently authored 0BSD NiteRex / Web9.X core and native XHTML projection.
# These targets do not claim a complete browser, certificate verification or
# historical archive certification. Keep existing parity and witness APIs intact.
if(WIN32)
  add_executable(niterex_web9x_xhtml src/niterex_web9x_xhtml_renderer.cpp)
  target_include_directories(niterex_web9x_xhtml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_compile_features(niterex_web9x_xhtml PRIVATE cxx_std_20)
  set_target_properties(niterex_web9x_xhtml PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  install(TARGETS niterex_web9x_xhtml RUNTIME DESTINATION bin)
endif()
include(CTest)
if(BUILD_TESTING)
    add_executable(niterex_web9x_ninebit_test tests/niterex_web9x_ninebit_test.cpp)
    target_include_directories(niterex_web9x_ninebit_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(niterex_web9x_ninebit_test PRIVATE cxx_std_20)
    set_target_properties(niterex_web9x_ninebit_test PROPERTIES
      CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME niterex_web9x_ninebit_exhaustive
      COMMAND niterex_web9x_ninebit_test)
    if(TARGET pentagonal_xor_std_free)
      add_test(NAME parity_std_free_example COMMAND
        pentagonal_xor_std_free 0.80 0.70 0.90 0.60 0.95)
      set_tests_properties(parity_std_free_example PROPERTIES
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 51\\.72800000%")
      add_test(NAME parity_std_free_all_zero COMMAND
        pentagonal_xor_std_free 0 0 0 0 0)
      set_tests_properties(parity_std_free_all_zero PROPERTIES
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 0\\.00000000%")
      add_test(NAME parity_std_free_odd COMMAND
        pentagonal_xor_std_free 1 1 1 0 0)
      set_tests_properties(parity_std_free_odd PROPERTIES
        PASS_REGULAR_EXPRESSION "Pentagonal XOR: 100\\.00000000%")
      add_test(NAME parity_std_free_invalid_range COMMAND
        pentagonal_xor_std_free 0.8 0.7 1.2 0.6 0.95)
      set_tests_properties(parity_std_free_invalid_range PROPERTIES WILL_FAIL TRUE)
      add_test(NAME parity_std_free_invalid_nan COMMAND
        pentagonal_xor_std_free 0.8 0.7 nan 0.6 0.95)
      set_tests_properties(parity_std_free_invalid_nan PROPERTIES WILL_FAIL TRUE)
      add_test(NAME parity_std_free_invalid_arity COMMAND
        pentagonal_xor_std_free 0.8 0.7 0.9 0.6)
      set_tests_properties(parity_std_free_invalid_arity PROPERTIES WILL_FAIL TRUE)
    endif()
    add_executable(pentagonal_suite_test
        tests/pentagonal_suite_test.cpp)
    target_link_libraries(pentagonal_suite_test
        PRIVATE swe_pentagonal_xor_throughput)
    set_target_properties(pentagonal_suite_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME pentagonal_suite_operations
        COMMAND pentagonal_suite_test)
    if(TARGET pentagonal_suite_cli)
      add_test(NAME pentagonal_suite_cli_smoke
          COMMAND pentagonal_suite_cli)
      set_tests_properties(pentagonal_suite_cli_smoke PROPERTIES
          PASS_REGULAR_EXPRESSION "Suite pipeline processed in")
    endif()

    add_executable(pentagonal_xor_throughput_test
        tests/pentagonal_xor_throughput_test.cpp)
    target_link_libraries(pentagonal_xor_throughput_test
        PRIVATE swe_pentagonal_xor_throughput)
    set_target_properties(pentagonal_xor_throughput_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME pentagonal_xor_swar_roundtrip
        COMMAND pentagonal_xor_throughput_test)
    if(TARGET pentagonal_xor_cli)
      add_test(NAME pentagonal_xor_throughput_cli_smoke
          COMMAND pentagonal_xor_cli --bytes 65536)
      set_tests_properties(pentagonal_xor_throughput_cli_smoke PROPERTIES
          PASS_REGULAR_EXPRESSION "periodic 256-byte LUT")
    endif()

    # Isolated canonical parity object for machine-level zero-CALL disassembly.
    # Does not alter the production CLI or create an alternate algorithm.
    add_library(swe_zero_call_core_probe OBJECT tests/zero_call_core_probe.cpp)
    target_compile_features(swe_zero_call_core_probe PRIVATE cxx_std_20)
    set_target_properties(swe_zero_call_core_probe PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

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

    # Exploratory punctuation lexicon: exact byte identity, no mathematical
    # execution of unresolved operators. Independent of production parity.
    add_executable(quantum_operand_lexicon_test
        tests/quantum_operand_lexicon_test.cpp)
    target_compile_features(quantum_operand_lexicon_test PRIVATE cxx_std_20)
    set_target_properties(quantum_operand_lexicon_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME quantum_operand_exact_symbol_registry
        COMMAND quantum_operand_lexicon_test)

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

    # COREAO exact GammaGamma and toolong tests: part of the standard
    # CMake/CTest matrix on Linux, macOS, MSVC and supported ARM targets.
    add_executable(gammagamma_pi_precision_test
        tests/gammagamma_pi_precision_test.cpp)
    target_compile_features(gammagamma_pi_precision_test PRIVATE cxx_std_20)
    set_target_properties(gammagamma_pi_precision_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME gammagamma_pi_precision_boundaries
        COMMAND gammagamma_pi_precision_test)

    add_executable(toolong_arithmetic_test tests/toolong_arithmetic_test.cpp)
    target_compile_features(toolong_arithmetic_test PRIVATE cxx_std_20)
    set_target_properties(toolong_arithmetic_test PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME toolong_exact_arithmetic
        COMMAND toolong_arithmetic_test)

    if(MSVC)
        target_compile_options(parity_reference_test PRIVATE /W4 /permissive-)
        target_compile_options(deca_sigma_test PRIVATE /W4 /permissive-)
        target_compile_options(tri_pi_test PRIVATE /W4 /permissive-)
        target_compile_options(coreao_test PRIVATE /W4 /permissive-)
        target_compile_options(coreao_gain_test PRIVATE /W4 /permissive-)
        target_compile_options(counterarccosine_secondwave_test PRIVATE /W4 /permissive-)
        target_compile_options(stability10d_test PRIVATE /W4 /permissive-)
        target_compile_options(coreo_stereo_test PRIVATE /W4 /permissive-)
        target_compile_options(gammagamma_pi_precision_test PRIVATE /O2 /W4 /permissive-)
        target_compile_options(toolong_arithmetic_test PRIVATE /O2 /W4 /permissive-)
    else()
        target_compile_options(parity_reference_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(deca_sigma_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(tri_pi_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreao_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreao_gain_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(counterarccosine_secondwave_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(stability10d_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(coreo_stereo_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(gammagamma_pi_precision_test PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(toolong_arithmetic_test PRIVATE -Wall -Wextra -Wpedantic)
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
    shared/euler_pi_inverse_square.hpp
    shared/sigma_form_geometry.hpp
    shared/sigma_form_area_q62.hpp
    shared/right_triangle_f123_original.hpp
    shared/f123_eighth_fold_experiment.hpp
    shared/sigma_e_redaxualchoice.hpp
    shared/sigma_f_redaxualchoice.hpp
    shared/four_pyramid_reduxualchoice.hpp
    shared/u_circle_fit.hpp
    shared/minza_topology_incidence.hpp
    shared/xdr_rdx_fold_tork.hpp
    shared/minza_direct_ping_bidi.hpp
    shared/minza_direct_ping_tridi.hpp
    shared/three_geometric_folds.hpp
    shared/rectangloid_primitive_cxx20.hpp
    shared/toolong_q62_core.hpp
    shared/slidingtoolong.hpp
    shared/eulers_jig_canonical.hpp
    shared/coreao.hpp
    shared/coreao_gain.hpp
    shared/gammagamma.hpp
    shared/pi_precision.hpp
    shared/toolong.hpp
    shared/toolong_ratio.hpp
    shared/toolong_bits.hpp
    shared/toolong_q62.hpp
    shared/toolong_photon_model.hpp
    shared/stability10d.hpp
    shared/coreo_stereo.hpp
    shared/pentagonal_xor_std_free.h
    DESTINATION include/swe)

install(TARGETS pentagonal_xor RUNTIME DESTINATION bin)
if(MSVC)
    install(FILES "$<TARGET_PDB_FILE:pentagonal_xor>" DESTINATION symbols OPTIONAL)
endif()
install(FILES README.md LICENSE docs/DECASIGMA.md docs/TRI_PI.md docs/COREAO.md docs/BUILD_EVIDENCE.md docs/RELEASE_PREFLIGHT.md docs/STABILITY10D.md docs/ANDROID.md docs/COREAO_GAIN.md docs/TOOLONG.md docs/GAMMAGAMMA_PI_PRECISION.md docs/COUNTERARCCOSINE_2NDWAVE.svg DESTINATION share/doc/pentagonal-xor)


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
  set_property(TARGET pentagonal_ligaze PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE FALSE)
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
    set_property(TARGET pentagonal_ligaze_benchmark PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
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
  shared/handi_sigmapi.hpp
  shared/handi_rot_triad.hpp
  shared/sigmadi_handecasigma_presentation.hpp
  shared/sigmadi_collision12.hpp
  DESTINATION include/swe)
install(FILES
  docs/SIGMA_PENTA_HAN_PRESENTATION.md
  docs/SIGMA_PENTA_HAN_VISTA_BOARD.svg
  DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(handi_sigmapi_test tests/handi_sigmapi_test.cpp)
  target_compile_features(handi_sigmapi_test PRIVATE cxx_std_20)
  set_target_properties(handi_sigmapi_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  if(MSVC)
    target_compile_options(handi_sigmapi_test PRIVATE /O2)
  else()
    target_compile_options(handi_sigmapi_test PRIVATE -O2)
  endif()
  add_test(NAME handi_sigmapi_observation_not_automatic_pass
           COMMAND handi_sigmapi_test)
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

# 12-axis declaration witness (exploratory, nonmutating on collision).
install(FILES docs/SIGMADI_COLLISION12.md
        DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(sigmadi_collision12_test tests/sigmadi_collision12_test.cpp)
  target_compile_features(sigmadi_collision12_test PRIVATE cxx_std_20)
  add_test(NAME sigmadi_collision12_declaration_custody
           COMMAND sigmadi_collision12_test)
endif()

# MINZA Pinwheel Atlas · Hanzeye 九眼: independent C++20 color/time audit.
# No modifications to canonical Pentagonal XOR, COR, or VISTA machinery.
install(FILES shared/MINZA_Color_Eyes_Gate_Core_CXX20.hpp DESTINATION include/swe)
install(FILES
  docs/MINZA_PINWHEEL_ATLAS.md
  docs/MINZA_COLOR_EYES_RGBA64.md
  docs/MINZA_TEMPORAL_PENTAGONAL.md
  docs/MINZA_FEMTOSECOND_GEOMETRY.md
  DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(minza_color_eyes_test tests/minza_color_eyes_test.cpp)
  target_compile_features(minza_color_eyes_test PRIVATE cxx_std_20)
  set_target_properties(minza_color_eyes_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  add_test(NAME minza_color_eyes_rgba64_temporal_integrity
           COMMAND minza_color_eyes_test)
endif()

# Verify experimental nine-loop terminal labels separately from native FLOP.
if(BUILD_TESTING)
  add_test(NAME vista9x9_separate_terminal_route
           COMMAND pentagonal_vista9x9 0x17 0x1f)
  set_tests_properties(vista9x9_separate_terminal_route PROPERTIES
    PASS_REGULAR_EXPRESSION "terminal_flops=5.*terminal_flips=4.*final_reversed=0")
endif()

# Ship all public documentation in the same flat directory as the register,
# including HANDI, the writing procedure and other sibling-linked pages.
# The original files remain in docs/ with their Git history unchanged.
install(DIRECTORY docs/
  DESTINATION share/doc/pentagonal-xor
  FILES_MATCHING PATTERN "*.md" PATTERN "*.svg")

# Ship the copyright/0BSD/marks boundary and the outstanding review audit.
install(FILES
  docs/LICENSE_AND_MARKS.md
  docs/DOCUMENT_REVIEW_REGISTER.md
  DESTINATION share/doc/pentagonal-xor)

# Independent NINZA (copy comparison) and PING (permutation inverse) adapter.
# The original Sigma-PENTA producer and six-stage Machine remain unchanged.
install(FILES shared/minza_ninza_ping_unfold.hpp DESTINATION include/swe)
install(FILES docs/NINZA_PING_UNFORGE_UNTWIRL.md
        DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(minza_ninza_ping_unfold_test tests/minza_ninza_ping_unfold_test.cpp)
  target_compile_features(minza_ninza_ping_unfold_test PRIVATE cxx_std_20)
  set_target_properties(minza_ninza_ping_unfold_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  add_test(NAME minza_ninza_ping_54byte_inverse_and_mismatch
           COMMAND minza_ninza_ping_unfold_test)
  # Exact nine-face incidence and the scoped AC-hinge witness.
  add_executable(minza_topology_design_test tests/minza_topology_design_test.cpp)
  target_compile_features(minza_topology_design_test PRIVATE cxx_std_20)
  set_target_properties(minza_topology_design_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  target_compile_options(minza_topology_design_test PRIVATE
    /O2 /W4 /permissive- /constexpr:steps2000000)
  add_test(NAME minza_topology_design COMMAND minza_topology_design_test)

  # The original nine-lane byte route has a separate STL-backed API.
  add_executable(minza_byte_pipeline_test tests/minza_byte_pipeline_test.cpp)
  target_compile_features(minza_byte_pipeline_test PRIVATE cxx_std_20)
  set_target_properties(minza_byte_pipeline_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  target_compile_options(minza_byte_pipeline_test PRIVATE /O2 /W4 /permissive-)
  add_test(NAME minza_byte_pipeline_wildcard COMMAND minza_byte_pipeline_test)
endif()

# Distribute source-independent Dolphin subject-catalog instructions, not private media.
install(FILES docs/DOLPHIN_SUBJECT_SEARCH.md
        DESTINATION share/doc/pentagonal-xor)

# Source-free v12 centered-Centrix geometry and symmetric R10 illumination.
install(FILES shared/minza_centrix_light_cxx20.hpp DESTINATION include/swe)
install(FILES docs/MINZA_CENTRIX_NATIVE_DOM_V12.md
        DESTINATION share/doc/pentagonal-xor)
if(BUILD_TESTING)
  add_executable(minza_centrix_light_test tests/minza_centrix_light_test.cpp)
  target_compile_features(minza_centrix_light_test PRIVATE cxx_std_20)
  set_target_properties(minza_centrix_light_test PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  add_test(NAME minza_centrix_light_symmetry COMMAND minza_centrix_light_test)
endif()

# Opt-in host scheduler, outside STD-free core-only path.
option(SWE_NINZA_PING_EXECUTOR "Enable experimental host PING executor" OFF)
if(SWE_NINZA_PING_EXECUTOR)
  find_package(Threads REQUIRED)
  add_library(swe_ninza_ping_executor INTERFACE)
  target_compile_features(swe_ninza_ping_executor INTERFACE cxx_std_20)
  target_include_directories(swe_ninza_ping_executor INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/shared")
  target_link_libraries(swe_ninza_ping_executor INTERFACE Threads::Threads)
  if(BUILD_TESTING)
    add_executable(ninza_ping_scheduler_test tests/ninza_ping_scheduler_test.cpp)
    target_link_libraries(ninza_ping_scheduler_test PRIVATE swe_ninza_ping_executor)
    if(MSVC)
      target_compile_options(ninza_ping_scheduler_test PRIVATE /O2)
    else()
      target_compile_options(ninza_ping_scheduler_test PRIVATE -O2)
    endif()
    add_test(NAME ninza_ping_scheduler_explicit_order COMMAND ninza_ping_scheduler_test)
    set_tests_properties(ninza_ping_scheduler_explicit_order PROPERTIES TIMEOUT 20)
  endif()
endif()

# FIFONFU subprocess recovery is an opt-in host-side PowerShell fixture.
if(SWE_NINZA_PING_EXECUTOR AND BUILD_TESTING)
  find_program(SWE_NINZA_PWSH NAMES pwsh)
  if(SWE_NINZA_PWSH)
    add_test(NAME ninza_fifonfu_parent_supervision
      COMMAND "${SWE_NINZA_PWSH}" -NoLogo -NoProfile -NonInteractive
        -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test-NinzaFifonfu.ps1")
    set_tests_properties(ninza_fifonfu_parent_supervision PROPERTIES TIMEOUT 90)
  endif()
endif()

# Opt-in PiPi dual scheduling and idle-retirement fixture.
if(SWE_NINZA_PING_EXECUTOR AND BUILD_TESTING AND SWE_NINZA_PWSH)
  add_test(NAME ninza_pipi_star_supervision
    COMMAND "${SWE_NINZA_PWSH}" -NoLogo -NoProfile -NonInteractive
      -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test-NinzaPiPi.ps1")
  set_tests_properties(ninza_pipi_star_supervision PROPERTIES TIMEOUT 180)
endif()

# DIAERY native XHTML Strict-profile inspector (experimental).
# The root native-MSVC and release HOLD policies remain authoritative.
add_subdirectory(diaery)

# RECOVERY: type-only topology and zero-work regression; no new runtime scheduler.
install(FILES shared/recovery.hpp DESTINATION include/swe)
if(BUILD_TESTING)
  add_executable(recovery_test tests/recovery_test.cpp)
  target_compile_features(recovery_test PRIVATE cxx_std_20)
  add_test(NAME recovery_zero_work_and_identity COMMAND recovery_test)
endif()

# PHYSICALS: independent seeded swimming laboratory.
install(FILES shared/physicals_swimming.hpp DESTINATION include/swe)
if(BUILD_TESTING)
  add_executable(physicals_swimming_test tests/physicals_swimming_test.cpp)
  target_compile_features(physicals_swimming_test PRIVATE cxx_std_20)
  add_test(NAME physicals_swimming_900_seconds COMMAND physicals_swimming_test)
endif()

# SPHERROID: independent source-preserving radial projection.
install(FILES shared/spherroid.hpp DESTINATION include/swe)
if(BUILD_TESTING)
  add_executable(spherroid_test tests/spherroid_test.cpp)
  target_compile_features(spherroid_test PRIVATE cxx_std_20)
  add_test(NAME spherroid_radial_convergence COMMAND spherroid_test)
endif()

if(BUILD_TESTING)
  add_executable(inverse_square_profile_test tests/inverse_square_profile_test.cpp)
  target_compile_features(inverse_square_profile_test PRIVATE cxx_std_20)
  add_test(NAME inverse_square_profile_parity COMMAND inverse_square_profile_test)
endif()

# PHYSICS LAB: isolated numerical and discrete experimental formuliums.
install(FILES shared/physics_lab_geometry.hpp shared/physics_lab_formulium.hpp DESTINATION include/swe)
if(BUILD_TESTING)
  add_executable(physics_lab_formulium_test tests/physics_lab_formulium_test.cpp)
  target_compile_features(physics_lab_formulium_test PRIVATE cxx_std_20)
  set_target_properties(physics_lab_formulium_test PROPERTIES CXX_EXTENSIONS OFF)
  add_test(NAME physics_lab_formulium_integrity COMMAND physics_lab_formulium_test)
endif()
