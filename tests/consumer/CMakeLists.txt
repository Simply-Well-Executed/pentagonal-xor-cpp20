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
project(SWEAuditReleaseConsumer LANGUAGES CXX)
# A renamed Clang/AppleClang driver must not bypass the path gate.
if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang)$")
  message(FATAL_ERROR "TOOLCHAIN|HOLD|clang_family_compiler_prohibited")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED SWE_AUDIT_PREFIX)
    message(FATAL_ERROR "Set SWE_AUDIT_PREFIX to the installed release stage directory")
endif()

# Verify the exported public API as a downstream project would.
include("${SWE_AUDIT_PREFIX}/lib/cmake/SWEAuditHeaders/SWEAuditHeadersTargets.cmake")

add_executable(swe_audit_release_consumer main.cpp)
target_link_libraries(swe_audit_release_consumer PRIVATE swe::swe_audit_headers)
set_target_properties(swe_audit_release_consumer PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

enable_testing()
add_test(NAME consumer_uses_installed_tri_pi_and_coreao
         COMMAND swe_audit_release_consumer)
