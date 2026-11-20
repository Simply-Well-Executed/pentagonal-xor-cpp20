#!/bin/sh
set -eu
export LC_ALL=C
fail() { printf 'IRON|DOCKER|BLOCKED|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing installed local tool: $1"; }
test "$#" -eq 1 || fail 'usage: sh ci/Invoke-LocalDocker-Gamp4.sh linux-amd64|linux-arm64|linux-x86|android-ABI'
profile=$1
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'invalid repo path'
cd "$root" || fail 'unable to enter source root'
for tool in git docker grep id tar sha256sum find sort awk; do need "$tool"; done
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon transport not permitted' ;; esac
endpoint=$(docker context inspect --format '{{.Endpoints.docker.Host}}' 2>/dev/null) ||
  fail 'cannot inspect Docker daemon context'
case "$endpoint" in unix://*|npipe://*) : ;; *) fail 'Docker context must be a local Unix/npipe endpoint' ;; esac
docker info >/dev/null 2>&1 || fail 'local Docker daemon unavailable'
docker compose version >/dev/null 2>&1 || fail 'Docker Compose plugin missing'
git diff --quiet --exit-code HEAD -- || fail 'uncommitted tracked source changes'
test -z "$(git status --porcelain --untracked-files=all)" ||
  fail 'untracked source or uncommitted changes'
SWE_EXPECTED_SHA=$(git rev-parse --verify HEAD) || fail 'Git commit unavailable'
case "$SWE_EXPECTED_SHA" in
  *[!0-9a-fA-F]*|'') fail 'invalid commit SHA' ;;
esac
test "${#SWE_EXPECTED_SHA}" -eq 40 || fail 'source must be a SHA-1 Git commit'
export SWE_EXPECTED_SHA
SWE_RELEASE_OUTPUT="${SWE_RELEASE_OUTPUT:-$root/build/gamp4-release/$SWE_EXPECTED_SHA}"
case "$SWE_RELEASE_OUTPUT" in /*) : ;; *) fail 'SWE_RELEASE_OUTPUT must be an absolute path' ;; esac
mkdir -p "$SWE_RELEASE_OUTPUT" || fail 'cannot create local artifact root'
SWE_SOURCE_STAGING_DIR="$SWE_RELEASE_OUTPUT/_source/$SWE_EXPECTED_SHA"
snapshot_root="$SWE_RELEASE_OUTPUT/_source"
snapshot_manifest="$snapshot_root/$SWE_EXPECTED_SHA.sha256"
snapshot_tar="$snapshot_root/$SWE_EXPECTED_SHA.tar"
mkdir -p "$snapshot_root" || fail 'cannot create source snapshot directory'
if test ! -d "$SWE_SOURCE_STAGING_DIR"; then
  mkdir -p "$SWE_SOURCE_STAGING_DIR" || fail 'cannot stage Git source'
  git archive --format=tar HEAD > "$snapshot_tar" ||
    fail 'cannot export current committed source without network'
  tar -xf "$snapshot_tar" -C "$SWE_SOURCE_STAGING_DIR" ||
    fail 'cannot unpack committed source snapshot'
  printf '%s\n' "$SWE_EXPECTED_SHA" > "$SWE_SOURCE_STAGING_DIR/.swe-source-commit"
  (
    cd "$SWE_SOURCE_STAGING_DIR"
    find . -type f -exec sha256sum '{}' ';' | sort
  ) > "$snapshot_manifest" || fail 'cannot hash staged committed source'
fi
test -f "$snapshot_manifest" || fail 'snapshot checksum manifest missing'
test -f "$snapshot_tar" || fail 'source archive provenance missing'
test ! -e "$SWE_SOURCE_STAGING_DIR/.git" || fail 'Git metadata must not enter container'
test -z "$(find "$SWE_SOURCE_STAGING_DIR" -type l -print)" ||
  fail 'symlinks in container snapshot require separate review'
test "$(cat "$SWE_SOURCE_STAGING_DIR/.swe-source-commit")" = "$SWE_EXPECTED_SHA" ||
  fail 'source marker changed'
(
  cd "$SWE_SOURCE_STAGING_DIR"
  sha256sum -c "$snapshot_manifest" >/dev/null
) || fail 'committed source snapshot changed after export'
snapshot_files=$(find "$SWE_SOURCE_STAGING_DIR" -type f | awk 'END {print NR}')
manifest_files=$(awk 'END {print NR}' "$snapshot_manifest")
test "$snapshot_files" -eq "$manifest_files" ||
  fail 'source snapshot contains unmanifested files'
export SWE_SOURCE_STAGING_DIR
SWE_HOST_UID=$(id -u)
SWE_HOST_GID=$(id -g)
export SWE_RELEASE_OUTPUT SWE_HOST_UID SWE_HOST_GID
service=''
image=''
case "$profile" in
  linux-amd64) service=linux-gamp4; SWE_LINUX_PLATFORM=linux/amd64; SWE_LINUX_PROFILE="$profile" ;;
  linux-arm64) service=linux-gamp4; SWE_LINUX_PLATFORM=linux/arm64; SWE_LINUX_PROFILE="$profile" ;;
  linux-x86) service=linux-gamp4; SWE_LINUX_PLATFORM=linux/386; SWE_LINUX_PROFILE="$profile" ;;
  android-arm64-v8a) service=android-ndk; SWE_ANDROID_ABI=arm64-v8a ;;
  android-armeabi-v7a) service=android-ndk; SWE_ANDROID_ABI=armeabi-v7a ;;
  android-x86) service=android-ndk; SWE_ANDROID_ABI=x86 ;;
  android-x86_64) service=android-ndk; SWE_ANDROID_ABI=x86_64 ;;
  *) fail 'macOS/Windows binaries must be built by their native host; no false Docker OS emulation' ;;
esac
if test "$service" = linux-gamp4; then
  image="${SWE_LINUX_BUILDER_IMAGE:-}"
  test -n "$image" || fail 'SWE_LINUX_BUILDER_IMAGE must name a preloaded digest'
  SWE_ANDROID_BUILDER_IMAGE="${SWE_ANDROID_BUILDER_IMAGE:-$image}"
  SWE_ANDROID_NDK_DIR="${SWE_ANDROID_NDK_DIR:-$root}"
else
  image="${SWE_ANDROID_BUILDER_IMAGE:-}"
  test -n "$image" || fail 'SWE_ANDROID_BUILDER_IMAGE must name a preloaded digest'
  SWE_LINUX_BUILDER_IMAGE="${SWE_LINUX_BUILDER_IMAGE:-$image}"
  SWE_ANDROID_NDK_DIR="${SWE_ANDROID_NDK_DIR:-}"
  test -n "$SWE_ANDROID_NDK_DIR" || fail 'installed Android NDK directory required'
  test -f "$SWE_ANDROID_NDK_DIR/build/cmake/android.toolchain.cmake" ||
    fail 'Android NDK file missing on host'
fi
printf '%s\n' "$image" | grep -Eq '@sha256:[0-9a-fA-F]{64}$' ||
  fail 'unpinned builder image; require exact @sha256 digest'
docker image inspect "$image" >/dev/null 2>&1 ||
  fail 'builder image digest not cached locally; download prohibited'
receipt="$SWE_RELEASE_OUTPUT/evidence/$profile"
mkdir -p "$receipt" || fail 'cannot write receipt directory'
if test -e "$receipt/gamp4-gate.yml" || test -e "$receipt/cross-gate.yml"; then
  fail 'existing evidence for profile; choose a new output batch to preserve prior run'
fi
export SWE_LINUX_BUILDER_IMAGE SWE_ANDROID_BUILDER_IMAGE
export SWE_ANDROID_NDK_DIR SWE_LINUX_PLATFORM SWE_LINUX_PROFILE SWE_ANDROID_ABI
log="$receipt/docker-console.log"
if ! docker compose -f "$root/docker/compose.release.yml" run \
        --rm --no-deps "$service" > "$log" 2>&1; then
  cat "$log" >&2
  fail "container command failed; evidence preserved: $log"
fi
cat "$log"
if test "$service" = linux-gamp4; then
  test -f "$receipt/gamp4-gate.yml" || fail 'missing GAMP4 evidence'
  grep -Fxq "status: 'PASS'" "$receipt/gamp4-gate.yml" ||
    fail 'GAMP4 gate not successful'
else
  test -f "$receipt/cross-gate.yml" || fail 'Android ABI evidence missing'
  grep -Fxq "status: 'CROSS_COMPILED_NOT_DEVICE_TESTED'" "$receipt/cross-gate.yml" ||
    fail 'Android ABI not cross-compiled'
fi
printf 'IRON|DOCKER|EVIDENCE_RECORDED|%s|%s\n' "$profile" "$SWE_EXPECTED_SHA"
