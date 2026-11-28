# Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
# 
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# SPDX-License-Identifier: 0BSD
# Verify a separately approved SHA-256 manifest. Never generate or update it here.
# Format: lowercase SHA256, two spaces, repository-relative path, one per line.
cmake_minimum_required(VERSION 3.23)
if(NOT IS_DIRECTORY "${ROOT}" OR NOT EXISTS "${MANIFEST}" OR NOT EXISTS "${GIT}")
  message(FATAL_ERROR "BAKED source manifest or local inventory unavailable")
endif()
execute_process(COMMAND "${GIT}" -C "${ROOT}" -c core.quotePath=false ls-files --cached
  RESULT_VARIABLE rc OUTPUT_VARIABLE tracked ERROR_VARIABLE err)
if(NOT "${rc}" STREQUAL "0")
  message(FATAL_ERROR "BAKED local inventory failed: ${err}")
endif()
string(REPLACE "\r\n" "\n" tracked "${tracked}")
string(REGEX REPLACE "\n$" "" tracked "${tracked}")
if(tracked MATCHES "[;\r]" OR tracked MATCHES "(^|\n)\"")
  message(FATAL_ERROR "BAKED unsupported quoted or delimiter path")
endif()
string(REPLACE "\n" ";" tracked "${tracked}")
list(SORT tracked)
file(STRINGS "${MANIFEST}" records ENCODING UTF-8)
set(seen "")
foreach(record IN LISTS records)
  if(NOT record MATCHES "^([0-9a-f]+)  (.+)$")
    message(FATAL_ERROR "BAKED invalid source record")
  endif()
  set(expected "${CMAKE_MATCH_1}")
  set(path "${CMAKE_MATCH_2}")
  string(LENGTH "${expected}" length)
  if(NOT length EQUAL 64 OR IS_ABSOLUTE "${path}" OR
     path MATCHES "(^|/)\\.\\.?(/|$)" OR path MATCHES "[;\\\\:]" OR
     NOT EXISTS "${ROOT}/${path}" OR IS_DIRECTORY "${ROOT}/${path}" OR
     IS_SYMLINK "${ROOT}/${path}")
    message(FATAL_ERROR "BAKED invalid or missing path: ${path}")
  endif()
  file(REAL_PATH "${ROOT}" realroot)
  file(REAL_PATH "${ROOT}/${path}" realpath)
  cmake_path(IS_PREFIX realroot "${realpath}" NORMALIZE inside)
  if(NOT inside)
    message(FATAL_ERROR "BAKED path outside source root: ${path}")
  endif()
  if(path IN_LIST seen)
    message(FATAL_ERROR "BAKED duplicate record: ${path}")
  endif()
  file(SHA256 "${ROOT}/${path}" observed)
  if(NOT observed STREQUAL expected)
    message(FATAL_ERROR "BAKED source changed: ${path}")
  endif()
  list(APPEND seen "${path}")
endforeach()
list(SORT seen)
if(NOT "${seen}" STREQUAL "${tracked}")
  message(FATAL_ERROR "BAKED manifest does not cover exact tracked inventory")
endif()
list(LENGTH seen count)
message(STATUS "BAKED source manifest verified: ${count} tracked files")
