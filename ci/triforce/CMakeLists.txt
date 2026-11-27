# SPDX-License-Identifier: 0BSD
cmake_minimum_required(VERSION 3.23)
if(NOT CMAKE_HOST_WIN32)
  message(FATAL_ERROR "TRIFORCE|HOLD|native_Windows_MSVC_only")
endif()
project(SWETriforceHeadTail LANGUAGES CXX)
if(NOT MSVC OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "TRIFORCE|HOLD|native_MSVC_not_clang_cl")
endif()
# WONTHAI/1: isolate the precision-test configuration from the root project.
set(SWE_TRIFORCE_LANE "CORE" CACHE STRING "CORE or PRECISION")
if(NOT SWE_TRIFORCE_LANE STREQUAL "CORE" AND
   NOT SWE_TRIFORCE_LANE STREQUAL "PRECISION")
  message(FATAL_ERROR "TRIFORCE|HOLD|unknown_WONTHAI_lane")
endif()
if(SWE_TRIFORCE_LANE STREQUAL "PRECISION")
  enable_testing()
  foreach(test_target IN ITEMS toolong_arithmetic_test gammagamma_pi_precision_test)
    add_executable(${test_target} ../../tests/${test_target}.cpp)
    set_target_properties(${test_target} PROPERTIES
      CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    target_compile_options(${test_target} PRIVATE /O2 /W4 /permissive-)
  endforeach()
  add_test(NAME toolong_exact_arithmetic COMMAND toolong_arithmetic_test)
  add_test(NAME gammagamma_pi_precision_boundaries COMMAND gammagamma_pi_precision_test)
  # Independent arithmetic diagnostics: same native toolchain, no root build.
  add_executable(lcd_test ../../tests/LCD_TEST.cpp)
  add_executable(talon_test ../../tests/TALON_TEST.cpp)
  add_executable(namespace_free_test ../../tests/namespace_free_test.cpp
    ../../docs/experiments/fuckitin_standalone.cpp
    ../../docs/experiments/zipitup_standalone.cpp
    ../../docs/experiments/zipitdown_standalone.cpp
    ../../docs/experiments/zipitout_standalone.cpp)
  foreach(test_target IN ITEMS lcd_test talon_test namespace_free_test)
    set_target_properties(${test_target} PROPERTIES
      CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    target_compile_options(${test_target} PRIVATE /O2 /W4 /permissive- /fp:precise /constexpr:steps1000000)
  endforeach()
  add_test(NAME lcd_256_cycles_exact_quotient COMMAND lcd_test)
  add_test(NAME talon_256_cycles_terminal_velocity COMMAND talon_test)
  add_test(NAME namespace_free_composition_65025 COMMAND namespace_free_test)
  return()
endif()
add_executable(triforce_native_gate ../../tests/triforce_native_gate_test.cpp)
set_target_properties(triforce_native_gate PROPERTIES
  CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(triforce_native_gate PRIVATE /O2 /W4 /permissive-)
enable_testing()
add_test(NAME triforce_toolong_q62_headtail COMMAND triforce_native_gate)
