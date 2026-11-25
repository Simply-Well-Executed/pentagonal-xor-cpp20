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

# Exact femtosecond timebase / rational major-subunit grid / bounded trace gate.
add_executable(swe_diaery_trace_timeline trace_timeline_test.cpp)
set_target_properties(swe_diaery_trace_timeline PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_trace_timeline PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_dynamo_fs_grid_trace_gate COMMAND swe_diaery_trace_timeline)

# PentaDi: four explicit stages and a shared NULL sentinel.
add_executable(swe_diaery_pentadi pentadi_test.cpp)
set_target_properties(swe_diaery_pentadi PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_pentadi PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_pentadi_shared_null COMMAND swe_diaery_pentadi)

# Research cohort eligibility: zero/null/nonpositive anatomy is excluded,
# source provenance remains available to the audit.
add_executable(swe_diaery_research_filter research_filter_test.cpp)
set_target_properties(swe_diaery_research_filter PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_research_filter PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_research_exclude_zero_source_preserved
 COMMAND swe_diaery_research_filter)

# FILTROX: research sample-range warnings for penile/vaginal measures;
# unlike FILTREX, sample extremum is not universal biological cutoff.
add_executable(swe_diaery_filtrox filtrox_test.cpp)
set_target_properties(swe_diaery_filtrox PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_filtrox PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_filtrox_study_range_both_anatomies
 COMMAND swe_diaery_filtrox)

# DYNTR0X: bounded 10-layer PROJECTIONS, no synthetic scientific estimates.
add_executable(swe_diaery_projections projections_test.cpp)
set_target_properties(swe_diaery_projections PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_projections PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_dyntr0x_projections_evidence_and_band
 COMMAND swe_diaery_projections)

# THAILONGA: anatomy, PROJECTIONS, source-null status and exact fs in one frame.
add_executable(swe_diaery_thailonga thailonga_test.cpp)
set_target_properties(swe_diaery_thailonga PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_thailonga PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_thailonga_shared_source_and_projections
 COMMAND swe_diaery_thailonga)

# THAILONGA FREQ: independent frequency counts and optional Gaussian fit.
add_executable(swe_diaery_freq freq_test.cpp)
set_target_properties(swe_diaery_freq PROPERTIES
 CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_freq PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_thailonga_freq_count_bell_null_gates
 COMMAND swe_diaery_freq)

add_executable(swe_diaery_circle_band circle_band_test.cpp)
set_target_properties(swe_diaery_circle_band PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_circle_band PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_thailonga_circle_band_evidence_gate COMMAND swe_diaery_circle_band)

add_executable(swe_diaery_quintiles quintiles_test.cpp)
set_target_properties(swe_diaery_quintiles PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_quintiles PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_thailonga_quintiles_evidence_gate COMMAND swe_diaery_quintiles)

add_executable(swe_diaery_audit audit_test.cpp)
set_target_properties(swe_diaery_audit PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_diaery_audit PRIVATE /O2 /W4 /permissive-)
add_test(NAME diaery_thailonga_audit_evidence_gate COMMAND swe_diaery_audit)
