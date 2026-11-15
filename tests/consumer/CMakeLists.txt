cmake_minimum_required(VERSION 3.20)
project(SWEAuditReleaseConsumer LANGUAGES CXX)

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
