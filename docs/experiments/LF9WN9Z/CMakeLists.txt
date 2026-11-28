# SPDX-License-Identifier: 0BSD
cmake_minimum_required(VERSION 3.30)
project(LF9WN9Z LANGUAGES CXX)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "LF9WN9Z requires native MSVC")
endif()
add_library(LF9WN9Z SHARED LF9WN9Z.cpp LF9WN9Z.hpp)
set_target_properties(LF9WN9Z PROPERTIES CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_definitions(LF9WN9Z PRIVATE LF9WN9Z_BUILD)
target_compile_options(LF9WN9Z PRIVATE /O2 /W4 /permissive-)
