#!/bin/sh
set -eu
export LC_ALL=C
fail() { printf 'IRON|MACOS|FAIL|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing tool: $1"; }
test "$#" -eq 1 || fail 'usage: sh ci/Run-Native-Macos-Gamp4.sh macos-arm64|macos-x86_64|macos-universal'
profile=$1
case "$profile" in
  macos-arm64) arch=arm64; host_required=arm64 ;;
  macos-x86_64) arch=x86_64; host_required=x86_64 ;;
  macos-universal) arch='arm64;x86_64'; host_required=any ;;
  *) fail "unsupported macOS profile: $profile" ;;
esac
test "$(uname -s)" = Darwin || fail 'native macOS host required; not Docker emulation'
cpu=$(uname -m)
if test "$host_required" != any && test "$cpu" != "$host_required"; then
  fail "macOS native test requires host $host_required, got $cpu"
fi
for tool in git cmake ctest xcrun grep awk find sort shasum; do need "$tool"; done
xcrun --find clang++ >/dev/null || fail 'installed Xcode/CLT Clang missing'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source directory'
cd "$root" || fail 'source directory'
git diff --quiet --exit-code HEAD -- || fail 'uncommitted tracked source'
test -z "$(git status --porcelain --untracked-files=all)" || fail 'dirty/untracked source'
head=$(git rev-parse --verify HEAD) || fail 'Git source missing'
test "${#head}" -eq 40 || fail 'invalid source SHA'
out="${SWE_RELEASE_OUTPUT:-$root/build/gamp4-release/$head}"
case "$out" in /*) : ;; *) fail 'absolute output directory required' ;; esac
mkdir -p "$out/evidence/$profile"
dir="$out/evidence/$profile"
test ! -e "$dir/gamp4-gate.yml" || fail 'existing evidence: choose a separate run batch'
if test "$profile" = macos-universal; then
  for requirement in macos-arm64 macos-x86_64; do
    prior="$out/evidence/$requirement/gamp4-gate.yml"
    test -f "$prior" || fail "native architecture GAMP4 evidence missing: $requirement"
    grep -Fxq "source_commit: '$head'" "$prior" || fail "$requirement SHA differs"
    grep -Fxq "status: 'PASS'" "$prior" || fail "$requirement did not pass"
  done
fi
state=FAIL
stage=preflight
code=1
ctest_state=NOT_TESTED
summary=NOT_TESTED
binary_sha=NOT_TESTED
receipt="$dir/gamp4-gate.yml"
record() {
  {
    printf "schema: SWE-IRON-GAMP4-NATIVE-MACOS-v1\n"
    printf "source_commit: '%s'\n" "$head"
    printf "profile: '%s'\n" "$profile"
    printf "native_host_cpu: '%s'\n" "$cpu"
    printf "stage: '%s'\n" "$stage"
    printf "status: '%s'\n" "$state"
    printf "exit_code: %s\n" "$code"
    printf "ctest_status: '%s'\n" "$ctest_state"
    printf "gamp4_summary: '%s'\n" "$summary"
    printf "binary_sha256: '%s'\n" "$binary_sha"
    printf "qa_status: 'NOT_APPROVED'\n"
    printf "pq_status: 'NOT_PERFORMED'\n"
  } > "$receipt.tmp"
  mv -f "$receipt.tmp" "$receipt"
}
trap 'code=$?; if test "$code" -eq 0; then state=PASS; fi; record; exit "$code"' 0
build="$out/_build/$profile/full"
stage=configure
cmake -S "$root" -B "$build" -DCMAKE_BUILD_TYPE=Release \
  "-DCMAKE_OSX_ARCHITECTURES=$arch" -DBUILD_TESTING=ON \
  -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DCMAKE_CXX_EXTENSIONS=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  > "$dir/configure.log" 2>&1 || fail 'macOS full configure failed'
stage=compile
cmake --build "$build" --config Release --parallel 2 \
  > "$dir/build.log" 2>&1 || fail 'macOS full build failed'
stage=ctest
ctest --test-dir "$build" -C Release --output-on-failure \
  > "$dir/ctest.log" 2>&1 || fail 'macOS CTest failed'
ctest_state=PASS
stage=gamp4-urs
exe="$build/pentagonal_gamp4_validation_test"
test -f "$exe" || fail 'GAMP4 executable absent'
"$exe" > "$dir/urs.log" 2>&1 || fail 'GAMP4 failed on macOS'
for n in 11 12 13 14 15 16 17 18 19 20 21 22; do
  count=$(grep -c "^PASS|URS-$n|" "$dir/urs.log" || :)
  test "$count" -eq 1 || fail "missing/duplicated URS-$n"
done
count=$(grep -c '^PASS|URS-' "$dir/urs.log" || :)
test "$count" -eq 12 || fail "unexpected URS PASS count: $count"
if grep -q '^FAIL|' "$dir/urs.log"; then fail 'failed URS log'; fi
grep -Fxq 'GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0' "$dir/urs.log" ||
  fail 'GAMP4 summary absent'
summary='PASS|cases=12|failures=0'
binary_sha=$(shasum -a 256 "$exe" | awk '{print $1}')
stage=core-archive
core="$out/_build/$profile/core"
cmake -S "$root" -B "$core" "-DCMAKE_OSX_ARCHITECTURES=$arch" \
  -DCMAKE_BUILD_TYPE=Release -DSWE_STD_FREE_CORE_ONLY=ON -DBUILD_TESTING=OFF \
  -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=OFF \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON > "$dir/core-configure.log" 2>&1 ||
  fail 'macOS core configure failed'
cmake --build "$core" --config Release --target swe_std_free_portable_core --parallel 2 \
  > "$dir/core-build.log" 2>&1 || fail 'macOS core compilation failed'
package="$out/packages/$profile"
mkdir -p "$package/bin"
cmake --install "$core" --prefix "$package" --config Release \
  > "$dir/install.log" 2>&1 || fail 'macOS installation failed'
for name in pentagonal_xor pentagonal_xor_std_free pentagonal_xor_cli pentagonal_suite_cli; do
  test -f "$build/$name" || fail "missing executable $name"
  cp "$build/$name" "$package/bin/$name"
done
stage=checksums
(
  cd "$package"
  find . -type f -exec shasum -a 256 '{}' ';' | sort > "$dir/package-sha256sums.txt"
) || fail 'macOS package hashes unavailable'
stage=complete
printf 'IRON|MACOS|PASS|%s|%s|URS=12\n' "$profile" "$head"
