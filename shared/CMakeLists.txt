cmake_minimum_required(VERSION 3.20)
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
project(SWETrustBoundaryGuard LANGUAGES C CXX)
# PR #32 native MSVC-only (clang-cl is NOT MSVC).
if(NOT CMAKE_HOST_WIN32 OR NOT MSVC OR
   NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|native_MSVC_only")
endif()
if(NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|native_MSVC_C_only")
endif()
# A renamed Clang/AppleClang driver must not bypass the path gate.
if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang)$")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|clang_family_compiler_prohibited")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Produce usable debug symbols even in optimized Release builds.
# MSVC PDBs are distributable outputs of this project, not the compiler itself.
if(MSVC)
    add_compile_options(/Zi)
    add_link_options(/DEBUG:FULL)
else()
    add_compile_options(-g)
endif()

add_executable(swe_guard swe_guard.cpp)
target_compile_features(swe_guard PRIVATE cxx_std_20)
set_target_properties(swe_guard PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

# The std-free public header is also compiled as strict C11. This verifies
# that it does not accidentally depend on C++ syntax or standard headers.
add_executable(pentagonal_xor_std_free_test tests/std_free_test.c)
set_target_properties(pentagonal_xor_std_free_test PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
  C_EXTENSIONS OFF
)
if(MSVC)
  target_compile_options(swe_guard PRIVATE /W4 /permissive-)
  target_compile_options(pentagonal_xor_std_free_test PRIVATE /W4)
else()
  target_compile_options(swe_guard PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(pentagonal_xor_std_free_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME all_32_parity_states COMMAND swe_guard --self-test)
add_test(NAME valid_metadata COMMAND swe_guard https://simpwellx.com/ public/downloads/pentagonal-xor-emblem/color.svg aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa brand-color.svg)
add_test(NAME reject_untrusted_host COMMAND swe_guard https://evil.example/ public/downloads/pentagonal-xor-emblem/color.svg aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa brand-color.svg)
set_tests_properties(reject_untrusted_host PROPERTIES WILL_FAIL TRUE)
add_test(NAME reject_traversal COMMAND swe_guard https://simpwellx.com/ ../private.env aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa brand-color.svg)
set_tests_properties(reject_traversal PROPERTIES WILL_FAIL TRUE)
add_test(NAME std_free_c11_header COMMAND pentagonal_xor_std_free_test)
