#!/bin/sh
set -eu
export LC_ALL=C
fail() { printf 'IRON|GAMP4|FAIL|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing preinstalled tool: $1"; }
test "$#" -eq 1 || fail 'usage: Run-Gamp4-Container.sh linux-amd64|linux-arm64|linux-x86'
profile=$1
case "$profile" in linux-amd64|linux-arm64|linux-x86) : ;; *) fail 'non-Linux profile or invalid name' ;; esac
test -f /src/.swe-source-commit || fail 'sanitized Git archive source marker missing'
test ! -d /src/.git || fail 'Docker source must not expose .git metadata'
test -d /out || fail 'writable artifact directory must be mounted at /out'
test -n "${SWE_EXPECTED_SHA:-}" || fail 'SWE_EXPECTED_SHA missing'
for tool in cmake ctest sha256sum grep awk find sort cp cat; do need "$tool"; done
head=$(cat /src/.swe-source-commit) || fail 'source archive marker unreadable'
test "$head" = "$SWE_EXPECTED_SHA" || fail 'sanitized snapshot SHA differs from host'
output="/out/evidence/$profile"
mkdir -p "$output"
report="$output/gamp4-gate.yml"
stage=preflight
success=FAIL
code=1
record() {
  {
    printf "schema: SWE-IRON-GAMP4-DOCKER-v1\n"
    printf "source_commit: '%s'\n" "$head"
    printf "profile: '%s'\n" "$profile"
    printf "scope: 'GAMP4-style supplier validation; not regulated certification'\n"
    printf "stage: '%s'\n" "$stage"
    printf "status: '%s'\n" "$success"
    printf "exit_code: %s\n" "$code"
    printf "required_urs: 12\n"
    printf "gamp4_summary: '%s'\n" "${summary:-NOT_TESTED}"
    printf "ctest_status: '%s'\n" "${ctest_status:-NOT_TESTED}"
    printf "binary_sha256: '%s'\n" "${binary_sha:-NOT_TESTED}"
    printf "runtime_environment: 'Docker Linux; may be emulated, not host-native qualification'\n"
    printf "pq_status: 'NOT_PERFORMED'\n"
    printf "qa_status: 'NOT_APPROVED'\n"
  } > "$report.tmp"
  mv -f "$report.tmp" "$report"
}
trap 'code=$?; if test "$code" -eq 0; then success=PASS; fi; record; exit "$code"' 0
build="/out/_build/$profile/full"
stage=cmake-configure
cmake -S /src -B "$build" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF \
  > "$output/configure.log" 2>&1 || fail 'full CMake configure failed'
stage=full-compile
cmake --build "$build" --config Release --parallel 2 \
  > "$output/build.log" 2>&1 || fail 'full C++20 compile failed'
stage=full-ctest
ctest --test-dir "$build" -C Release --output-on-failure \
  > "$output/ctest.log" 2>&1 || {
    cat "$output/ctest.log" >&2
    fail 'full CTest failed'
  }
ctest_status=PASS
stage=gamp4-urs
binary="$build/pentagonal_gamp4_validation_test"
test -f "$binary" || fail 'mandatory GAMP4 executable absent'
"$binary" > "$output/urs.log" 2>&1 || {
  cat "$output/urs.log" >&2
  fail 'GAMP4 URS executable failed'
}
for number in 11 12 13 14 15 16 17 18 19 20 21 22; do
  found=$(grep -c "^PASS|URS-$number|" "$output/urs.log" || :)
  test "$found" -eq 1 || fail "missing or duplicated URS-$number PASS"
done
total=$(grep -c '^PASS|URS-' "$output/urs.log" || :)
test "$total" -eq 12 || fail "unexpected number of PASS rows: $total"
if grep -q '^FAIL|' "$output/urs.log"; then fail 'URS log contains FAIL'; fi
summary=$(grep -Fxc 'GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0' "$output/urs.log" || :)
test "$summary" -eq 1 || fail 'GAMP4 final summary missing or duplicated'
summary='PASS|cases=12|failures=0'
binary_sha=$(sha256sum "$binary" | awk '{print $1}')
stage=core-archive
core="/out/_build/$profile/core"
cmake -S /src -B "$core" -DCMAKE_BUILD_TYPE=Release \
  -DSWE_STD_FREE_CORE_ONLY=ON -DBUILD_TESTING=OFF \
  -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DCMAKE_CXX_EXTENSIONS=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  > "$output/core-configure.log" 2>&1 || fail 'core CMake configure failed'
cmake --build "$core" --config Release --target swe_std_free_portable_core \
  --parallel 2 > "$output/core-build.log" 2>&1 ||
  fail 'STD-free static archive compile failed'
package="/out/packages/$profile"
mkdir -p "$package"
cmake --install "$core" --config Release --prefix "$package" \
  > "$output/install.log" 2>&1 || fail 'core install failed'
mkdir -p "$package/bin"
for name in pentagonal_xor pentagonal_xor_std_free pentagonal_xor_cli pentagonal_suite_cli; do
  test -f "$build/$name" || fail "missing validated native CLI: $name"
  cp "$build/$name" "$package/bin/$name"
done
stage=checksums
(
  cd "$package"
  find . -type f -exec sha256sum '{}' ';' | sort > "$output/package-sha256sums.txt"
) || fail 'artifact checksums failed'
stage=complete
printf 'IRON|GAMP4|PASS|%s|%s|URS=12\n' "$profile" "$head"
