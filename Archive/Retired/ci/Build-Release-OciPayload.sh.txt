#!/bin/sh
set -eu
fail() { printf 'IRON|OCI|HOLD|%s\n' "$*" >&2; exit 1; }
test "$#" -eq 0 || fail 'usage: sh ci/Build-Release-OciPayload.sh'
for tool in git grep sha256sum docker; do
  command -v "$tool" >/dev/null 2>&1 || fail "missing local tool: $tool"
done
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source root missing'
cd "$root" || fail 'source root unavailable'
git diff --quiet --exit-code HEAD -- || fail 'source is dirty'
test -z "$(git status --porcelain --untracked-files=all)" || fail 'untracked source exists'
sha=$(git rev-parse --verify HEAD) || fail 'Git commit unavailable'
out="${SWE_RELEASE_OUTPUT:-$root/build/gamp4-release/$sha}"
case "$out" in /*) : ;; *) fail 'absolute output path required' ;; esac
stage="$out/release-ready/$sha"
receipt="$stage/RELEASE_GATE.yml"
test -f "$receipt" || fail 'release not assembled; mandatory GAMP4 receipts absent'
grep -Fxq "source_commit: '$sha'" "$receipt" || fail 'release source SHA mismatch'
grep -Fxq "profile_count: 13" "$receipt" || fail 'all OS/ABI artifacts missing'
grep -Fxq "status: 'STAGED_NOT_PUBLISHED'" "$receipt" ||
  fail 'release gate status not staged'
(
  cd "$stage"
  sha256sum -c SHA256SUMS >/dev/null
) || fail 'release file checksums changed'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon disallowed' ;; esac
endpoint=$(docker context inspect --format '{{.Endpoints.docker.Host}}' 2>/dev/null) ||
  fail 'Docker context not inspectable'
case "$endpoint" in unix://*|npipe://*) : ;; *) fail 'remote Docker context disallowed' ;; esac
docker info >/dev/null 2>&1 || fail 'local Docker daemon absent'
short=$(printf '%s' "$sha" | cut -c1-12)
tag="swe-gamp4-release-payload:sha-$short"
docker build --pull=false --network=none \
  --file "$root/docker/ReleasePayload.Dockerfile" \
  --build-arg "SOURCE_COMMIT=$sha" \
  --tag "$tag" "$stage" || fail 'OCI artifact packing failed'
image_id=$(docker image inspect --format '{{.Id}}' "$tag") ||
  fail 'OCI image inspect failed'
test -n "$image_id" || fail 'missing OCI image digest'
printf 'IRON|OCI|LOCAL_ONLY|%s|%s\n' "$tag" "$image_id"
