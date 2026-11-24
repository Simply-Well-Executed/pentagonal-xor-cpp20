# SPDX-License-Identifier: 0BSD
cmake_minimum_required(VERSION 3.23)
project(SWEExcitedAStar LANGUAGES CXX)
if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "EXCITED|HOLD|native_MSVC_required")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
foreach(configuration DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  set(CMAKE_CXX_FLAGS_${configuration} "/O2 /DNDEBUG /MD"
      CACHE STRING "EXCITED native O2 only" FORCE)
endforeach()
add_executable(swe_excited_astar astar_raw.cpp)
add_executable(swe_dovia_policy dovia_policy_test.cpp)
set_target_properties(swe_excited_astar PROPERTIES
  CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_excited_astar PRIVATE /O2 /W4 /WX /permissive- /EHsc /GR-)
target_link_libraries(swe_excited_astar PRIVATE kernel32)
set_target_properties(swe_dovia_policy PROPERTIES
  CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_options(swe_dovia_policy PRIVATE /O2 /W4 /WX /permissive- /EHsc /GR-)
include(CTest)
if(BUILD_TESTING)
  add_test(NAME excited_astar_exact_path COMMAND swe_excited_astar)
  add_test(NAME excited_spt_finland_verifier COMMAND swe_excited_astar)
  add_test(NAME excited_dovia_ruspt_guard_negative_tests COMMAND swe_dovia_policy)
  add_test(NAME excited_dovia_coreao_witness COMMAND swe_excited_astar)
  set_tests_properties(excited_astar_exact_path PROPERTIES
    PASS_REGULAR_EXPRESSION "Total path cost:\\*\\* 6")
  set_tests_properties(excited_dovia_coreao_witness PROPERTIES
    PASS_REGULAR_EXPRESSION "DOVIA YANG and RUSPT YIN verified")
  set_tests_properties(excited_spt_finland_verifier PROPERTIES
    PASS_REGULAR_EXPRESSION "SPT total goal cost:\\*\\* 6")
endif()
