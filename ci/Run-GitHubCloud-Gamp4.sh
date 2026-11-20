#!/bin/sh
# SPDX-License-Identifier: 0BSD
# GitHub-hosted-only Docker GAMP4. No user-local compute or publication.
set -eu
export LC_ALL=C
fail() { printf 'IRON|CLOUD_DOCKER|BLOCKED|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing GitHub runner prerequisite: $1"; }
for tool in git docker tar grep find awk sort sha256sum uname id cat; do need "$tool"; done
test "${GITHUB_ACTIONS:-}" = true || fail 'GitHub Actions required'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'self-hosted/local runner forbidden'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 ||
  fail 'unexpected repository'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'only explicit branch push is authorized'
test "${GITHUB_REF:-}" = refs/heads/feat/cloud-gamp4-docker-on-github-20261120 ||
  fail 'unexpected branch'
test "$(uname -m)" = x86_64 || fail 'native Linux/amd64 hosted runner required'
test -n "${GITHUB_SHA:-}" && test "${#GITHUB_SHA}" -eq 40 || fail 'Git SHA missing'
case "$GITHUB_SHA" in *[!0-9a-fA-F]*|'') fail 'invalid source SHA' ;; esac
test -n "${RUNNER_TEMP:-}" || fail 'hosted runner temporary directory unavailable'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon forbidden' ;; esac
endpoint=$(docker context inspect --format '{{.Endpoints.docker.Host}}') ||
  fail 'cannot inspect hosted daemon'
printf '%s\n' "$endpoint" | grep -Eq '^(unix|npipe)://' ||
  fail 'Docker context is not local to GitHub cloud runner'
docker info >/dev/null || fail 'cloud Docker daemon unavailable'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source checkout absent'
test "$(git -C "$root" rev-parse --verify HEAD)" = "$GITHUB_SHA" ||
  fail 'checkout SHA disagrees with GitHub event SHA'
test -z "$(git -C "$root" status --porcelain --untracked-files=all)" ||
  fail 'source checkout dirty'
workspace="$RUNNER_TEMP/swe-gamp4-$GITHUB_RUN_ID-${GITHUB_RUN_ATTEMPT:-1}"
source="$workspace/source/$GITHUB_SHA"
output="$workspace/output"
empty="$workspace/context"
mkdir -p "$source" "$output" "$empty"
git -C "$root" archive --format=tar HEAD | tar -xf - -C "$source" ||
  fail 'Git archive extraction failed'
printf '%s\n' "$GITHUB_SHA" > "$source/.swe-source-commit"
test ! -e "$source/.git" || fail 'source snapshot exposes .git'
test -z "$(find "$source" -type l -print)" || fail 'symlink in source snapshot'
(
  cd "$source"
  find . -type f -exec sha256sum '{}' ';' | sort > "$workspace/source.sha256"
  sha256sum -c "$workspace/source.sha256" >/dev/null
) || fail 'source SHA256 snapshot mismatch'
n=$(find "$source" -type f | awk 'END{print NR}')
m=$(awk 'END{print NR}' "$workspace/source.sha256")
test "$n" -eq "$m" && test "$n" -gt 1 || fail 'source manifest incomplete'

# Explicit cloud-only pull, resolve immutable digest before building toolchain.
# Docker container runtime remains network=none; apt install is build-only.
docker pull --platform linux/amd64 ubuntu:24.04 ||
  fail 'official base-image pull failed on GitHub hosted runner'
base=$(docker image inspect --format '{{index .RepoDigests 0}}' ubuntu:24.04) ||
  fail 'official base registry manifest unavailable'
printf '%s\n' "$base" | grep -Eq '@sha256:[0-9a-fA-F]{64}$' ||
  fail 'builder base did not resolve to a content-addressed registry digest'
printf 'IRON|CLOUD_DOCKER|BASE_REF|%s\n' "$base"
img="swe-gamp4-github:$GITHUB_SHA"
docker build --no-cache --pull=false --platform linux/amd64 \
  --build-arg "BASE_IMAGE=$base" \
  --file "$root/ci/docker/GitHubCloud-Gamp4.Dockerfile" \
  --tag "$img" "$empty" || fail 'cloud toolchain image build failed'
idimg=$(docker image inspect --format '{{.Id}}' "$img") ||
  fail 'cloud toolchain image missing'
printf '%s\n' "$idimg" | grep -Eq '^sha256:[0-9a-fA-F]{64}$' ||
  fail 'cloud toolchain image config identity invalid'
printf 'IRON|CLOUD_DOCKER|BUILDER_IMAGE_CONFIG_SHA|%s\n' "$idimg"

uid=$(id -u)
gid=$(id -g)
mkdir -p "$output/evidence/linux-amd64"
logs() {
  code=$?
  if test "$code" -ne 0; then
    printf 'IRON|CLOUD_DOCKER|FAIL|%s\n' "$code" >&2
    for name in docker-console.log configure.log build.log ctest.log urs.log \
          core-configure.log core-build.log install.log gamp4-gate.yml; do
      case "$name" in docker-console.log) file="$output/$name" ;;
        *) file="$output/evidence/linux-amd64/$name" ;;
      esac
      if test -f "$file"; then
        printf 'IRON|CLOUD_DOCKER|LOG|%s\n' "$name" >&2
        tail -n 180 "$file" >&2
      fi
    done
  fi
}
trap logs EXIT

docker run --rm --platform linux/amd64 \
  --network none --read-only --cap-drop ALL \
  --security-opt no-new-privileges --pids-limit 512 \
  --memory 6g --cpus 2 \
  --user "$uid:$gid" --workdir /src \
  --tmpfs /tmp:rw,nosuid,nodev,size=256m \
  --mount "type=bind,src=$source,dst=/src,readonly" \
  --mount "type=bind,src=$output,dst=/out" \
  --env "SWE_EXPECTED_SHA=$GITHUB_SHA" \
  --entrypoint /bin/sh "$idimg" \
  /src/ci/docker/Run-Gamp4-Container.sh linux-amd64 \
  > "$output/docker-console.log" 2>&1 ||
    fail 'Docker CMake/CTest/GAMP4-URS execution failed'

report="$output/evidence/linux-amd64/gamp4-gate.yml"
test -f "$report" || fail 'GAMP4 evidence missing'
grep -Fxq "status: 'PASS'" "$report" || fail 'GAMP4 receipt not PASS'
grep -Fxq "ctest_status: 'PASS'" "$report" || fail 'CTest receipt not PASS'
grep -Fxq "source_commit: '$GITHUB_SHA'" "$report" || fail 'source SHA mismatch'
ctest="$output/evidence/linux-amd64/ctest.log"
grep -Eq '100% tests passed, 0 tests failed out of [1-9][0-9]*' "$ctest" ||
  fail 'nonzero-test CTest acceptance missing'
urs="$output/evidence/linux-amd64/urs.log"
grep -Fxq 'GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0' "$urs" ||
  fail 'exact 12-case GAMP4 summary missing'
test -d "$output/packages/linux-amd64/lib" || fail 'native archive missing'
(
  cd "$output/packages/linux-amd64"
  sha256sum -c "$output/evidence/linux-amd64/package-sha256sums.txt" >/dev/null
) || fail 'native package SHA256 receipt mismatch'
cat "$ctest"
cat "$urs"
cat "$report"
printf 'IRON|CLOUD_DOCKER|PASS|%s|linux-amd64|no-release|no-push\n' "$GITHUB_SHA"
