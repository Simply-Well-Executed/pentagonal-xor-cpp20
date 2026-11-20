#!/bin/sh
# SPDX-License-Identifier: 0BSD
# Android/Linux project gate: reject Python executables and shipped scripts.
# Not a claim about Python on Android devices outside the delivered image.
set -eu
export LC_ALL=C
fail() { printf 'IRON|ANDROID_LINUX|PYTHON_FORBIDDEN|%s\n' "$*" >&2; exit 1; }
case "${1:-}" in source|image) mode=$1 ;; *) fail 'usage: sh ci/Assert-AndroidLinux-NoPython.sh source|image' ;; esac
if test "$mode" = source; then
  command -v git >/dev/null 2>&1 || fail 'source audit requires git'
  root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source root missing'
  cd "$root" || fail 'cannot enter source root'
  if git ls-files android ci docker include shared src tests |
      grep -Ei '(^|/)(__pycache__|.*[.]py[ic]?|.*[.]whl|requirements([-.].*)?[.]txt|pyproject[.]toml|pipfile([.]lock)?|poetry[.]lock|setup[.]py)$'; then
    fail 'a tracked Python script or package manifest is present'
  fi
  files='ci/Build-Android.ps1 ci/Build-AndroidUnsigned.ps1 ci/Initialize-AndroidRuntime.ps1 ci/Initialize-AndroidSdk.ps1 ci/Invoke-AndroidDockerValidation.ps1 ci/Invoke-LinuxDockerValidation.ps1 ci/docker/Build-Android-NDK.sh'
  for file in $files; do
    test -f "$file" || fail "missing required Android/Linux entrypoint: $file"
    if grep -niE '^[[:space:]]*(&[[:space:]]*)?([./[:alnum:]_-]+/)?(python([0-9.]+)?|pip([0-9.]+)?|pypy[0-9.]*|conda|poetry|pytest)([[:space:];|&)]|$)' "$file"; then
      fail "Python interpreter or package-manager invocation found: $file"
    fi
  done
  printf 'IRON|ANDROID_LINUX|SOURCE_NO_PYTHON|PASS\n'
else
  for executable in python python2 python3 python3.12 pip pip2 pip3 pypy pypy3 pipx uv poetry conda pytest; do
    if command -v "$executable" >/dev/null 2>&1; then
      fail "Python executable found on PATH: $executable"
    fi
  done
  for location in /usr/bin /usr/local/bin /bin /opt/venv/bin; do
    if test -d "$location"; then
      hits=$(find "$location" -maxdepth 1 \( -name 'python*' -o -name 'pip[0-9]*' -o -name 'pypy*' -o -name 'pytest' \) -print)
      test -z "$hits" || fail "Python interpreter or pip executable on disk: $hits"
    fi
  done
  if command -v dpkg-query >/dev/null 2>&1; then
    installed=$(dpkg-query -W -f='${binary:Package} ${db:Status-Abbrev}\n' 2>/dev/null |
      awk '$2 == "ii" && $1 ~ /^(python([0-9.]|-)|libpython[0-9.]|pypy|pipx)(:|$)/ {print $1}' || :)
    test -z "$installed" || fail "Python distribution packages remain installed: $installed"
  fi
  printf 'IRON|ANDROID_LINUX|IMAGE_NO_PYTHON|PASS\n'
fi
