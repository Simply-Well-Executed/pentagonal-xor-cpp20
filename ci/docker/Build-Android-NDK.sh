#!/bin/sh
set -eu
export LC_ALL=C
fail() { printf 'IRON|ANDROID|FAIL|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing local tool: $1"; }
test "$#" -eq 1 || fail 'usage: Build-Android-NDK.sh ABI'
abi=$1
case "$abi" in arm64-v8a|armeabi-v7a|x86|x86_64) : ;; *) fail 'unknown Android ABI' ;; esac
test -f /src/.swe-source-commit || fail 'sanitized source archive marker missing'
test ! -d /src/.git || fail 'Docker build cannot mount .git metadata'
test -d /out || fail 'artifact mount missing'
test -f /ndk/build/cmake/android.toolchain.cmake || fail 'read-only installed NDK mount missing'
test -n "${SWE_EXPECTED_SHA:-}" || fail 'expected source SHA missing'
for tool in cmake grep sha256sum awk find sort cat; do need "$tool"; done
head=$(cat /src/.swe-source-commit) || fail 'source archive SHA unreadable'
test "$head" = "$SWE_EXPECTED_SHA" || fail 'source commit mismatch'
source_gate=/out/evidence/linux-amd64/gamp4-gate.yml
test -f "$source_gate" || fail 'validated Linux AMD64 source evidence required first'
grep -Fxq "source_commit: '$head'" "$source_gate" || fail 'GAMP4 source commit differs'
grep -Fxq "status: 'PASS'" "$source_gate" || fail 'GAMP4 gate not PASS'
grep -Fxq "ctest_status: 'PASS'" "$source_gate" || fail 'GAMP4 CTest not PASS'
grep -Fxq "gamp4_summary: 'PASS|cases=12|failures=0'" "$source_gate" ||
  fail 'GAMP4 12-URS summary missing'
profile="android-$abi"
output="/out/evidence/$profile"
mkdir -p "$output"
stage=preflight
result=FAIL
code=1
report="$output/cross-gate.yml"
record() {
  {
    printf "schema: SWE-IRON-ANDROID-NDK-v1\n"
    printf "source_commit: '%s'\n" "$head"
    printf "profile: '%s'\n" "$profile"
    printf "gamp4_linux_source_gate: 'linux-amd64'\n"
    printf "stage: '%s'\n" "$stage"
    printf "status: '%s'\n" "$result"
    printf "exit_code: %s\n" "$code"
    printf "android_device_tests: 'NOT_TESTED'\n"
    printf "gamp4_android_oq: 'NOT_TESTED'\n"
    printf "runtime_binary_imports: 'NOT_TESTED'\n"
    printf "regulatory_certification: false\n"
  } > "$report.tmp"
  mv -f "$report.tmp" "$report"
}
trap 'code=$?; if test "$code" -eq 0; then result=CROSS_COMPILED_NOT_DEVICE_TESTED; fi; record; exit "$code"' 0
build="/out/_build/$profile/core"
stage=configure
cmake -S /src -B "$build" \
  -DCMAKE_TOOLCHAIN_FILE=/ndk/build/cmake/android.toolchain.cmake \
  "-DANDROID_ABI=$abi" -DANDROID_PLATFORM=android-24 \
  -DCMAKE_BUILD_TYPE=Release -DSWE_STD_FREE_CORE_ONLY=ON \
  -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  > "$output/configure.log" 2>&1 || fail 'NDK configure failed'
stage=compile
cmake --build "$build" --config Release --target swe_std_free_portable_core \
  --parallel 2 > "$output/build.log" 2>&1 || fail 'NDK archive build failed'
stage=install
package="/out/packages/$profile"
mkdir -p "$package"
cmake --install "$build" --prefix "$package" --config Release \
  > "$output/install.log" 2>&1 || fail 'NDK archive install failed'
stage=checksums
(
  cd "$package"
  find . -type f -exec sha256sum '{}' ';' | sort > "$output/package-sha256sums.txt"
) || fail 'checksum generation failed'
stage=complete
printf 'IRON|ANDROID|CROSS_COMPILED_NOT_DEVICE_TESTED|%s|%s\n' "$profile" "$head"
