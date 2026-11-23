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
