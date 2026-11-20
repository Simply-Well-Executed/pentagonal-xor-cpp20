#!/bin/sh
# SPDX-License-Identifier: 0BSD
# IRON T-CROSS: local POSIX gates + derived +/-DIFF YAML. No remote fetch.
# sh ci/t-cross-posix.sh [default|gate-name|emit-diff-yaml]
set -eu
LC_ALL=C
export LC_ALL
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || exit 1
cd "$root" || exit 1
: "${BUILD_DIR:=build}"
: "${BUILD_TYPE:=Release}"
: "${EXPECTED_PWSH_VERSION:=7.7.0}"
: "${EXPECTED_CMAKE_VERSION:=3.30.0}"
: "${T_CROSS_BASE_SHA:=313e25251243b85fb79f10c04236eb355beca1ce}"

fail() { printf 'T-CROSS|FAIL|%s\n' "$*" >&2; exit 1; }
pass() { printf 'T-CROSS|PASS|%s\n' "$*"; }
need() { command -v "$1" >/dev/null 2>&1 || fail "tool unavailable: $1"; }

gate_no_remote_git() {
    need git
    head=$(git rev-parse --verify HEAD) || fail 'local HEAD unavailable'
    git diff --quiet --exit-code HEAD -- || fail 'tracked source differs from HEAD'
    remotes=$(git remote) || fail 'cannot inspect Git remotes'
    test -z "$remotes" || fail "Git remote configured under strict policy: $remotes"
    pass "gate-no-remote-git|HEAD=$head"
}

gate_no_github_workflows() {
    for file in .github/workflows/*.yml .github/workflows/*.yaml; do
        test ! -e "$file" || fail "GitHub Actions workflow exists: $file"
    done
    pass 'gate-no-github-workflows'
}

gate_no_live_rest() {
    need find
    need grep
    pattern='Invoke-RestMethod|Invoke-WebRequest|HttpClient|DownloadFile|DownloadString|WebClient|Start-BitsTransfer|curl[[:space:]]|wget[[:space:]]|https?://api[.]github[.]com'
    hits=$(find ci tools include src -type f ! -name 't-cross-posix.sh' \
        \( -name '*.ps1' -o -name '*.sh' -o -name '*.cpp' -o -name '*.c' \
           -o -name '*.h' -o -name '*.hpp' -o -name '*.cmake' \
           -o -name '*.Dockerfile' \) \
        -exec grep -nE "$pattern" {} + 2>&1 || :)
    cmake_hits=$(grep -nE 'FetchContent|ExternalProject_Add|file[[:space:]]*[(][[:space:]]*DOWNLOAD|https?://api[.]github[.]com' CMakeLists.txt || :)
    if test -n "$hits" || test -n "$cmake_hits"; then
        printf '%s\n%s\n' "$hits" "$cmake_hits" >&2
        fail 'live-download / REST source candidate still present'
    fi
    pass 'gate-no-live-rest|strict-source-pattern-scan'
}

gate_no_residual_surfaces() {
    for dir in java docker android; do
        test ! -d "$dir" || fail "unreviewed residual source remains: $dir/"
    done
    pass 'gate-no-residual-surfaces'
}

gate_pinned_versions() {
    need cmake
    need awk
    need tr
    cmake_reply=$(cmake --version) || fail 'CMake version query failed'
    cmake_version=$(printf '%s\n' "$cmake_reply" | awk 'NR==1 {print $3}')
    test "$cmake_version" = "$EXPECTED_CMAKE_VERSION" ||
        fail "CMake $cmake_version != operator expected $EXPECTED_CMAKE_VERSION"
    pwsh_version='not-installed-POSIX-only'
    if command -v pwsh >/dev/null 2>&1; then
        pwsh_version=$(pwsh -NoLogo -NoProfile -Command '$PSVersionTable.PSVersion.ToString()') ||
            fail 'PowerShell version query failed'
        pwsh_version=$(printf '%s' "$pwsh_version" | tr -d '\r')
        test "$pwsh_version" = "$EXPECTED_PWSH_VERSION" ||
            fail "PowerShell $pwsh_version != operator expected $EXPECTED_PWSH_VERSION"
    fi
    pass "gate-pinned-versions|cmake=$cmake_version|pwsh=$pwsh_version|operator-asserted"
}

build_cpp20() {
    need cmake
    cmake -S . -B "$BUILD_DIR" \
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" \
        -DBUILD_TESTING=ON \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_CXX_EXTENSIONS=OFF \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF \
        -DFETCHCONTENT_FULLY_DISCONNECTED=ON ||
        fail 'CMake configure failed'
    cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel ||
        fail 'C++20 build failed'
    pass 'build-cpp20|ISO-C++20|extensions-OFF|optimization-from-CMake'
}

gate_zero_function_calls() {
    need find
    need grep
    need awk
    test -d "$BUILD_DIR" || fail 'build missing; run build-cpp20 first'
    objects=$(find "$BUILD_DIR" -type f \
        \( -name '*zero_call_core_probe*.obj' -o -name '*zero_call_core_probe*.o' \) -print) ||
        fail 'cannot enumerate parity probe objects'
    test -n "$objects" || fail 'isolated parity object missing'
    count=$(printf '%s\n' "$objects" | awk 'END {print NR}')
    test "$count" -eq 1 || fail "ambiguous parity objects: $count"
    if command -v llvm-objdump >/dev/null 2>&1; then
        asm=$(llvm-objdump -dr "$objects") || fail 'llvm-objdump failed'
    elif command -v objdump >/dev/null 2>&1; then
        asm=$(objdump -dr "$objects") || fail 'objdump failed'
    elif command -v dumpbin >/dev/null 2>&1; then
        asm=$(dumpbin /DISASM "$objects") || fail 'dumpbin failed'
    else
        fail 'disassembler missing, cannot establish zero-call'
    fi
    printf '%s\n' "$asm" | grep -q 'swe_pxor_zero_call_core' ||
        fail 'target symbol missing from disassembly'
    edges=$(printf '%s\n' "$asm" |
        grep -Ei '(^|[[:space:]])(call|callq|bl|blr|jal|jalr)([[:space:]]|$)' || :)
    if test -n "$edges"; then
        printf '%s\n' "$edges" >&2
        fail 'CALL instructions detected in optimized parity probe object'
    fi
    pass 'gate-zero-function-calls|scope=canonical-parity-object'
}

gate_zero_network_imports() {
    need find
    need grep
    need awk
    test -d "$BUILD_DIR" || fail 'build missing; run build-cpp20 first'
    for binary_name in pentagonal_xor pentagonal_xor_cli; do
        binaries=$(find "$BUILD_DIR" -type f \
            \( -name "$binary_name" -o -name "$binary_name.exe" \) -print) ||
            fail "cannot enumerate $binary_name binaries"
        test -n "$binaries" || fail "$binary_name binary missing"
        count=$(printf '%s\n' "$binaries" | awk 'END {print NR}')
        test "$count" -eq 1 || fail "ambiguous $binary_name binaries: $count"
        if command -v llvm-objdump >/dev/null 2>&1; then
            imports=$(llvm-objdump -p "$binaries") || fail 'llvm-objdump import scan failed'
        elif command -v objdump >/dev/null 2>&1; then
            imports=$(objdump -p "$binaries") || fail 'objdump import scan failed'
        elif command -v dumpbin >/dev/null 2>&1; then
            imports=$(dumpbin /IMPORTS "$binaries") || fail 'dumpbin import scan failed'
        elif command -v otool >/dev/null 2>&1; then
            imports=$(otool -L "$binaries") || fail 'otool import scan failed'
        else
            fail 'binary import inspection tool missing'
        fi
        hits=$(printf '%s\n' "$imports" |
            grep -Ei 'ws2_32|winhttp|wininet|urlmon|mswsock|libcurl|libssl|libcrypto|libsocket|CFNetwork' || :)
        if test -n "$hits"; then
            printf '%s\n' "$hits" >&2
            fail "known networking-capable direct import in $binary_name"
        fi
        pass "gate-zero-network-imports|binary=$binary_name|direct-imports-only"
    done
}

test_ctest() {
    need ctest
    ctest --test-dir "$BUILD_DIR" -C "$BUILD_TYPE" --output-on-failure ||
        fail 'CTest failed'
    pass 'test-ctest'
}

emit_diff_yaml() {
    need git
    need awk
    git cat-file -e "$T_CROSS_BASE_SHA^{commit}" ||
        fail "baseline commit not present locally: $T_CROSS_BASE_SHA"
    head=$(git rev-parse --verify HEAD) || fail 'local HEAD missing'
    printf "schema: 'SWE-T-CROSS-DIFF-v1'\n"
    printf "authority: 'local-Git-committed-line-diff'\n"
    printf "before: '%s'\nafter: '%s'\n" "$T_CROSS_BASE_SHA" "$head"
    printf "runtime_status: 'NOT_TESTED_BY_SOURCE_DIFF'\n"
    numstat=$(git diff --numstat "$T_CROSS_BASE_SHA" "$head" --) ||
        fail 'committed Git diff failed'
    printf '%s\n' "$numstat" | awk -F '\t' '
          BEGIN { print "files:"; add=0; rem=0; incomplete=0; count=0 }
          NF>=3 {
            path=$3; gsub(/\\/, "\\\\", path); gsub(/"/, "\\\"", path)
            print "  - path: \"" path "\""
            if ($1=="-" || $2=="-") {
              print "    lines: NOT_COUNTED_BINARY"; incomplete=1
            } else {
              printf "    added: %d\n    removed: %d\n", $1, $2
              add+=$1; rem+=$2
            }
            count++
          }
          END {
            printf "summary:\n  files: %d\n  lines_added: %d\n  lines_removed: %d\n  net: %d\n", count, add, rem, add-rem
            printf "  binary_line_counts_incomplete: %s\n", incomplete ? "true" : "false"
          }
        '
}

default() {
    emit_diff_yaml > "$root/.iron-t-cross-diff.yml"
    pass 'emit-diff-yaml|local-artifact=.iron-t-cross-diff.yml'
    gate_no_remote_git
    gate_no_github_workflows
    gate_no_live_rest
    gate_no_residual_surfaces
    gate_pinned_versions
    build_cpp20
    gate_zero_function_calls
    gate_zero_network_imports
    test_ctest
    pass 'IRON|NATIVE|ALL-DEFINED-GATES-PASSED'
}

case "${1:-default}" in
  default) default ;;
  gate-no-remote-git) gate_no_remote_git ;;
  gate-no-github-workflows) gate_no_github_workflows ;;
  gate-no-live-rest) gate_no_live_rest ;;
  gate-no-residual-surfaces) gate_no_residual_surfaces ;;
  gate-pinned-versions) gate_pinned_versions ;;
  build-cpp20) build_cpp20 ;;
  gate-zero-function-calls) gate_zero_function_calls ;;
  gate-zero-network-imports) gate_zero_network_imports ;;
  test-ctest) test_ctest ;;
  emit-diff-yaml) emit_diff_yaml ;;
  *) fail "unknown T-CROSS task: $1" ;;
esac
