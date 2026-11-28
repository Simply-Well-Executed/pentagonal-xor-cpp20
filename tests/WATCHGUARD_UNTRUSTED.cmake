# Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
# 
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# SPDX-License-Identifier: 0BSD
# Check the actual process exit value, not merely nonzero status.
execute_process(COMMAND "${BASELINE}" --bytes 0 RESULT_VARIABLE base OUTPUT_VARIABLE baseout ERROR_VARIABLE baseerr)
if(NOT "${base}" STREQUAL "0" OR NOT baseout MATCHES "periodic 256-byte LUT")
  message(FATAL_ERROR "Baseline did not complete: ${base} ${baseerr}")
endif()
execute_process(COMMAND "${PROGRAM}" --bytes 0 RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT "${result}" STREQUAL "79" OR NOT out MATCHES "periodic 256-byte LUT")
  message(FATAL_ERROR "UNTRUSTED normal route mismatch: ${result} ${err}")
endif()
execute_process(COMMAND "${BASELINE}" --invalid RESULT_VARIABLE basebad ERROR_VARIABLE basebaderr)
if(NOT "${basebad}" STREQUAL "2" OR NOT basebaderr MATCHES "Usage:")
  message(FATAL_ERROR "Baseline invalid route mismatch: ${basebad}")
endif()
execute_process(COMMAND "${PROGRAM}" --invalid RESULT_VARIABLE bad ERROR_VARIABLE baderr)
if(NOT "${bad}" STREQUAL "79" OR NOT baderr MATCHES "Usage:")
  message(FATAL_ERROR "UNTRUSTED invalid route mismatch: ${bad}")
endif()
message(STATUS "UNTRUSTED marker 79 observed on both routes; not a success verdict")
