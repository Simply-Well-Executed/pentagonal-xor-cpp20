# SPDX-License-Identifier: 0BSD
cmake_minimum_required(VERSION 3.30.0)
# Root project uses an existing native MSVC-only compiler policy.
# This subproject does not fetch sources or waive root approval gates.
if(NOT WIN32 OR NOT MSVC OR
   NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "DIAERY|HOLD|native_MSVC_only")
endif()
add_executable(swe_diaery diaery.cpp)
set_target_properties(swe_diaery PROPERTIES
  CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery PRIVATE /O2 /W4 /permissive-)
target_link_libraries(swe_diaery PRIVATE user32 gdi32 kernel32)
add_test(NAME diaery_xhtml_strict_1to1_concurrence
  COMMAND swe_diaery --check ${CMAKE_CURRENT_SOURCE_DIR}/index.xhtml)
add_test(NAME diaery_negative_doctype
  COMMAND swe_diaery --reject ${CMAKE_CURRENT_SOURCE_DIR}/bad_doctype.xhtml)
add_test(NAME diaery_negative_mismatched_witness
  COMMAND swe_diaery --reject ${CMAKE_CURRENT_SOURCE_DIR}/bad_parity.xhtml)
add_test(NAME diaery_negative_stage_mutation
  COMMAND swe_diaery --reject ${CMAKE_CURRENT_SOURCE_DIR}/bad_stage.xhtml)
add_test(NAME diaery_negative_active_script
  COMMAND swe_diaery --reject ${CMAKE_CURRENT_SOURCE_DIR}/bad_script.xhtml)

# Qualification tests are separate from basic 1:1 parity.
add_test(NAME diaery_fusion_seven_operator_qualification
  COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/index.xhtml)

# DIAERY-FUSION-WATCHDOG-PR34-016: negative XHTML qualified inputs.
add_test(NAME diaery_fusion_watchdog_negative_missing
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_watchdog_missing.xhtml)
set_tests_properties(diaery_fusion_watchdog_negative_missing
 PROPERTIES WILL_FAIL TRUE)
add_test(NAME diaery_fusion_watchdog_negative_mutated
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_watchdog_mutated.xhtml)
set_tests_properties(diaery_fusion_watchdog_negative_mutated
 PROPERTIES WILL_FAIL TRUE)
add_test(NAME diaery_fusion_watchdog_negative_duplicate
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_watchdog_duplicate.xhtml)
set_tests_properties(diaery_fusion_watchdog_negative_duplicate
 PROPERTIES WILL_FAIL TRUE)
add_test(NAME diaery_fusion_watchdog_negative_order
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_watchdog_order.xhtml)
set_tests_properties(diaery_fusion_watchdog_negative_order
 PROPERTIES WILL_FAIL TRUE)

# HARMONY-PR34-018: targeted corrupted XHTML witness.
add_test(NAME diaery_harmony_negative_missing
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_harmony_missing.xhtml)
set_tests_properties(diaery_harmony_negative_missing
 PROPERTIES WILL_FAIL TRUE)

# HARMONY-PR34-018: targeted corrupted XHTML witness.
add_test(NAME diaery_harmony_negative_mutated
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_harmony_mutated.xhtml)
set_tests_properties(diaery_harmony_negative_mutated
 PROPERTIES WILL_FAIL TRUE)

# HARMONY-PR34-018: targeted corrupted XHTML witness.
add_test(NAME diaery_harmony_negative_order
 COMMAND swe_diaery --qualify ${CMAKE_CURRENT_SOURCE_DIR}/bad_harmony_order.xhtml)
set_tests_properties(diaery_harmony_negative_order
 PROPERTIES WILL_FAIL TRUE)

# DIAERY finite-rational NASCAR qualification: separate from UI rendering.
add_executable(swe_diaery_finite head_to_head.cpp)
set_target_properties(swe_diaery_finite PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_finite PRIVATE /O2 /W4 /permissive-)
target_link_libraries(swe_diaery_finite PRIVATE kernel32)
add_test(NAME diaery_finite_slidingtoolong_head_to_head
 COMMAND swe_diaery_finite)

# UNBOUND-PR34-020: dynamic-limb C++20 native qualification.
add_executable(swe_diaery_unbound unbound_head_to_head.cpp)
set_target_properties(swe_diaery_unbound PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_unbound PRIVATE /O2 /W4 /permissive-)
target_link_libraries(swe_diaery_unbound PRIVATE kernel32)
add_test(NAME diaery_unbound_dynamic_limb_128
 COMMAND swe_diaery_unbound --steps 128)

# Native Strict-profile triangle/circle/loop concurrence witness.
add_executable(swe_diaery_triflavors tri_flavors.cpp)
set_target_properties(swe_diaery_triflavors PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_triflavors PRIVATE /O2 /W4 /permissive-)
target_link_libraries(swe_diaery_triflavors PRIVATE kernel32)
add_test(NAME diaery_triangle_triflavors_cpp_xhtml
 COMMAND swe_diaery_triflavors ${CMAKE_CURRENT_SOURCE_DIR}/tri_flavors.xhtml)
# Oracle Java 27.0.1.1 is a SEPARATE externally provisioned
# toolchain, never an implicit replacement or automatic download.

# APPLICATION OF Y=MX+B: compile-time source preservation and geometry gate.
add_executable(swe_diaery_application_of_ymxb application_of_ymxb_test.cpp)
set_target_properties(swe_diaery_application_of_ymxb PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_application_of_ymxb PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_application_of_ymxb_circle_line_minting
 COMMAND swe_diaery_application_of_ymxb)

# SIGMA uncertainty gate: fail closed on NULL, incomparable, or invalid model.
add_executable(swe_diaery_error_ledger error_ledger_test.cpp)
set_target_properties(swe_diaery_error_ledger PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_error_ledger PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_sigma_null_error_gate
 COMMAND swe_diaery_error_ledger)

# DYNAMO repaired XY/Z subject/provenance boundary.
add_executable(swe_diaery_dynamo_subject dynamo_subject_test.cpp)
set_target_properties(swe_diaery_dynamo_subject PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_dynamo_subject PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_dynamo_xy_z_source_gate COMMAND swe_diaery_dynamo_subject)
