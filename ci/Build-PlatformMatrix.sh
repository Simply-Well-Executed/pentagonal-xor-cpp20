#!/bin/sh
set -eu
LC_ALL=C
export LC_ALL

fail() { printf 'IRON|MATRIX|FAIL|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing local tool: $1"; }
test "$#" -eq 1 || fail 'usage: sh ci/Build-PlatformMatrix.sh <profile>'
profile=$1
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'repository path'
cd "$root" || fail 'repository directory'
manifest="$root/ci/native-platform-matrix.yml"
test -f "$manifest" || fail 'platform matrix YAML missing'
need grep
grep -Fqx "  $profile:" "$manifest" || fail "profile not declared in matrix: $profile"
need uname
host=$(uname -s)
cpu=$(uname -m)
apple_arch=''
android_abi=''
case "$profile" in
  macos-arm64) test "$host" = Darwin || fail 'requires local macOS'; apple_arch=arm64 ;;
  macos-x86_64) test "$host" = Darwin || fail 'requires local macOS'; apple_arch=x86_64 ;;
  macos-universal) test "$host" = Darwin || fail 'requires local macOS'; apple_arch='arm64;x86_64' ;;
  macos-i386) fail '32-bit macOS apps unsupported on modern macOS' ;;
  linux-x86_64)
    test "$host" = Linux || fail 'requires native Linux'
    case "$cpu" in x86_64|amd64) : ;; *) fail "CPU $cpu is not native x86_64" ;; esac ;;
  linux-arm64)
    test "$host" = Linux || fail 'requires native Linux'
    case "$cpu" in aarch64|arm64) : ;; *) fail "CPU $cpu is not native arm64" ;; esac ;;
  linux-x86)
    test "$host" = Linux || fail 'requires native Linux'
    case "$cpu" in i386|i486|i586|i686) : ;; *) fail "CPU $cpu is not native 32-bit x86" ;; esac ;;
  android-arm64-v8a) android_abi=arm64-v8a ;;
  android-armeabi-v7a) android_abi=armeabi-v7a ;;
  android-x86) android_abi=x86 ;;
  android-x86_64) android_abi=x86_64 ;;
  windows-*) fail 'use local PowerShell ci/Build-PlatformMatrix.ps1 with installed MSVC' ;;
  *) fail "unsupported local profile: $profile" ;;
esac

need cmake
need git
git diff --quiet --exit-code HEAD -- || fail 'tracked source dirty; commit/review changes'
commit=$(git rev-parse --verify HEAD) || fail 'local HEAD missing'
test "${#commit}" -eq 40 || fail 'unexpected Git SHA'
work="$root/build/iron-matrix/$profile"
mkdir -p "$work" || fail 'cannot create local build directory'
stage=preflight
report="$work/iron-build-result.yml"
on_exit() {
    result=$1
    if test "$result" -eq 0; then state=ARCHIVE_BUILT_NOT_RUNTIME_TESTED
    else state=FAIL; fi
    {
      printf "schema: SWE-IRON-PLATFORM-RESULT-v1\n"
      printf "profile: '%s'\n" "$profile"
      printf "source_commit: '%s'\n" "$commit"
      printf "host_os: '%s'\n" "$host"
      printf "host_cpu: '%s'\n" "$cpu"
      printf "stage: '%s'\n" "$stage"
      printf "status: '%s'\n" "$state"
      printf "scope: 'std-free-core-archive-only'\n"
      printf "runtime_tests: 'NOT_TESTED'\n"
      printf "full_repository_std_free: 'NOT_CERTIFIED'\n"
      printf "downloads_executed_by_script: 0\n"
    } > "$report"
}
trap 'rc=$?; on_exit "$rc"; exit "$rc"' 0

common_dir="$work/configured"
stage=configure
if test -n "$apple_arch"; then
    need xcrun
    xcrun --find clang++ >/dev/null || fail 'local Apple Clang missing'
    cmake -S "$root" -B "$common_dir" \
      -DCMAKE_BUILD_TYPE=Release -DSWE_STD_FREE_CORE_ONLY=ON \
      -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=20 \
      -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF \
      -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
      "-DCMAKE_OSX_ARCHITECTURES=$apple_arch" || fail 'macOS configure failed'
elif test -n "$android_abi"; then
    ndk="${ANDROID_NDK_HOME:-${ANDROID_NDK_ROOT:-}}"
    test -n "$ndk" || fail 'ANDROID_NDK_HOME is required, no SDK download'
    toolchain="$ndk/build/cmake/android.toolchain.cmake"
    test -f "$toolchain" || fail 'installed Android NDK toolchain missing'
    cmake -S "$root" -B "$common_dir" \
      "-DCMAKE_TOOLCHAIN_FILE=$toolchain" \
      "-DANDROID_ABI=$android_abi" \
      -DANDROID_PLATFORM=android-24 \
      -DCMAKE_BUILD_TYPE=Release -DSWE_STD_FREE_CORE_ONLY=ON \
      -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=20 \
      -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF \
      -DFETCHCONTENT_FULLY_DISCONNECTED=ON || fail 'Android configure failed'
else
    cmake -S "$root" -B "$common_dir" \
      -DCMAKE_BUILD_TYPE=Release -DSWE_STD_FREE_CORE_ONLY=ON \
      -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=20 \
      -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF \
      -DFETCHCONTENT_FULLY_DISCONNECTED=ON || fail 'Linux configure failed'
fi

stage=compile
cmake --build "$common_dir" --config Release \
  --target swe_std_free_portable_core --parallel || fail 'archive build failed'
stage=install
cmake --install "$common_dir" --prefix "$work/artifacts" ||
  fail 'archive installation failed'
stage=complete
printf 'IRON|MATRIX|ARCHIVE_BUILT_NOT_RUNTIME_TESTED|%s|%s\n' "$profile" "$commit"
