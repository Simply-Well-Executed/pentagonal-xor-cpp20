#!/bin/sh
set -eu
export LC_ALL=C
fail() { printf 'IRON|RELEASE|HOLD|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing local tool: $1"; }
test "$#" -eq 0 || fail 'usage: sh ci/Assemble-Gamp4-Release.sh'
for tool in git grep sha256sum tar find sort awk; do need "$tool"; done
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source root'
cd "$root" || fail 'source root'
git diff --quiet --exit-code HEAD -- || fail 'tracked source edits'
test -z "$(git status --porcelain --untracked-files=all)" || fail 'untracked source'
head=$(git rev-parse --verify HEAD) || fail 'source HEAD missing'
test "${#head}" -eq 40 || fail 'unexpected Git commit identifier'
out="${SWE_RELEASE_OUTPUT:-$root/build/gamp4-release/$head}"
case "$out" in /*) : ;; *) fail 'absolute SWE_RELEASE_OUTPUT required' ;; esac
test -d "$out/evidence" || fail 'validation receipts absent'
test -d "$out/packages" || fail 'built packages absent'
stage="$out/release-ready/$head"
test ! -e "$stage" || fail 'stage already exists; never overwrite prior evidence'
profiles='linux-amd64 linux-arm64 linux-x86 macos-arm64 macos-x86_64 macos-universal windows-x86 windows-x64 windows-arm64 android-arm64-v8a android-armeabi-v7a android-x86 android-x86_64'
for profile in $profiles; do
  receipt="$out/evidence/$profile"
  package="$out/packages/$profile"
  test -d "$package" || fail "package missing: $profile"
  test -f "$receipt/package-sha256sums.txt" || fail "checksums missing: $profile"
  test -n "$(find "$package" -type f -print -quit)" || fail "empty package: $profile"
  case "$profile" in
    android-*)
      file="$receipt/cross-gate.yml"
      test -f "$file" || fail "Android receipt missing: $profile"
      grep -Fxq "source_commit: '$head'" "$file" || fail "source mismatch: $profile"
      grep -Fxq "status: 'CROSS_COMPILED_NOT_DEVICE_TESTED'" "$file" ||
        fail "Android compile blocked: $profile"
      grep -Fxq "gamp4_linux_source_gate: 'linux-amd64'" "$file" ||
        fail "Android source GAMP4 provenance missing: $profile"
      ;;
    *)
      file="$receipt/gamp4-gate.yml"
      test -f "$file" || fail "native GAMP4 receipt missing: $profile"
      grep -Fxq "source_commit: '$head'" "$file" || fail "source mismatch: $profile"
      grep -Fxq "status: 'PASS'" "$file" || fail "GAMP4 blocked: $profile"
      grep -Fxq "ctest_status: 'PASS'" "$file" || fail "CTest blocked: $profile"
      grep -Fxq "gamp4_summary: 'PASS|cases=12|failures=0'" "$file" ||
        fail "12-URS summary missing: $profile"
      ;;
  esac
  (cd "$package" && sha256sum -c "$receipt/package-sha256sums.txt" >/dev/null) ||
    fail "file checksum mismatch: $profile"
  actual=$(find "$package" -type f | awk 'END {print NR}')
  expected=$(awk 'END {print NR}' "$receipt/package-sha256sums.txt")
  test "$actual" -eq "$expected" || fail "changed package manifest: $profile"
done

mkdir -p "$stage"
notes="$stage/RELEASE_NOTES.md"
{
  printf '# Pentagonal XOR: GAMP4-gated multi-OS release candidate\n\n'
  printf 'Source commit: %s\n\n' "$head"
  printf 'Validation class: supplier GAMP4-style automated tests, NOT GMP certification.\n\n'
  printf '| Target | Validation scope | Runtime environment |\n'
  printf '| --- | --- | --- |\n'
  for profile in $profiles; do
    case "$profile" in
      android-*) state='NDK compiled after Linux source GAMP4'; environment='Android device tests NOT PERFORMED' ;;
      macos-universal) state='GAMP4 CTest; both architecture receipts required'; environment='Native Xcode host' ;;
      macos-*) state='Full CTest and 12/12 GAMP4 URS'; environment='Native macOS host' ;;
      windows-*) state='Full CTest and 12/12 GAMP4 URS'; environment='Native Windows host' ;;
      linux-*) state='Full CTest and 12/12 GAMP4 URS'; environment='Offline Linux Docker, possibly CPU-emulated' ;;
    esac
    printf '| %s | %s | %s |\n' "$profile" "$state" "$environment"
  done
  printf '\nmacOS i386 is unsupported on modern macOS. '
  printf 'Windows AMD64 and x86-64 are aliases of the x64 architecture.\n\n'
  printf 'Each archive is linked to the source SHA, test evidence and package SHA256 manifest. '
  printf 'Library-free core source does not prove the absence of system/CRT imports.\n\n'
  printf 'External publication NOT PERFORMED. Production PQ and independent QA approval NOT PERFORMED.\n'
} > "$notes"
for profile in $profiles; do
  (cd "$out/packages/$profile" && tar -czf "$stage/pentagonal-xor-$profile-$head.tar.gz" .) ||
    fail "archive failed: $profile"
done
{
  printf "schema: SWE-IRON-GAMP4-RELEASE-STAGE-v1\n"
  printf "source_commit: '%s'\n" "$head"
  printf "profile_count: 13\n"
  printf "validation_scope: 'supplier-style GAMP4; PQ and QA signoff not performed'\n"
  printf "status: 'STAGED_NOT_PUBLISHED'\n"
  printf "regulatory_certification: false\n"
  printf "qa_approval_status: 'NOT_APPROVED'\n"
} > "$stage/RELEASE_GATE.yml"
(
  cd "$stage"
  find . -type f ! -name SHA256SUMS -exec sha256sum '{}' ';' |
    sort > SHA256SUMS
  sha256sum -c SHA256SUMS >/dev/null
) || fail 'release checksums failed'
printf 'IRON|RELEASE|STAGED_NOT_PUBLISHED|%s|profiles=13\n' "$head"
