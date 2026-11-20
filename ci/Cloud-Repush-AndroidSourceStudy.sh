#!/bin/sh
# SPDX-License-Identifier: 0BSD
# GitHub-hosted idempotent Docker Hub re-push. NOT a new GAMP4-qualified release.
set -eu
export LC_ALL=C
fail() { printf 'IRON|DOCKERHUB_REPUSH|HOLD|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "cloud runner tool missing: $1"; }
for name in git docker jq grep uname mktemp; do need "$name"; done
test "${GITHUB_ACTIONS:-}" = true || fail 'GitHub Actions required'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'self-hosted/local compute rejected'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 || fail 'unexpected repo'
test "${GITHUB_REF:-}" = refs/heads/feat/cloud-dockerhub-repush-20261120 || fail 'not approved cloud re-push branch'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'only an explicit repository push starts this job'
test "$(uname -m)" = x86_64 || fail 'expected GitHub-hosted Linux amd64'
test -n "${GITHUB_SHA:-}" || fail 'missing source commit'
test -n "${RUNNER_TEMP:-}" || fail 'missing hosted temp storage'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'missing source checkout'
test "$(git -C "$root" rev-parse HEAD)" = "$GITHUB_SHA" || fail 'unverified source SHA'
test -z "$(git -C "$root" status --porcelain --untracked-files=all)" || fail 'modified checkout'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon not allowed' ;; esac
endpoint=$(docker context inspect --format '{{.Endpoints.docker.Host}}') || fail 'cannot inspect cloud Docker context'
printf '%s\n' "$endpoint" | grep -Eq '^(unix|npipe)://' || fail 'daemon not local to GitHub runner'
docker info >/dev/null || fail 'GitHub hosted Docker engine unavailable'
docker buildx version >/dev/null || fail 'Docker buildx missing'

repo=paulcheek/android-source-study-v1
alias_tag=$repo:sha-07eb2a4561f0
target=$repo:latest
expected=sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30

inspect_json() {
  docker buildx imagetools inspect "$1" --format '{{json .Manifest}}' || fail "Docker Hub manifest read failed: $1"
}
manifest_digest() {
  printf '%s\n' "$1" | jq -er '.digest // empty' || fail 'registry manifest has no digest'
}
manifest_type() {
  printf '%s\n' "$1" | jq -er '.mediaType // empty' || fail 'registry manifest has no mediaType'
}

# A fresh Docker Hub registry read determines whether the requested
# historical image and current latest really resolve to identical bytes.
source_json=$(inspect_json "$alias_tag")
source_sha=$(manifest_digest "$source_json")
source_type=$(manifest_type "$source_json")
printf 'IRON|DOCKERHUB_REPUSH|SOURCE|%s|%s\n' "$alias_tag" "$source_sha"
test "$source_sha" = "$expected" || fail 'historical immutable source differs from supplied digest'
case "$source_type" in
  application/vnd.docker.distribution.manifest.v2+json|application/vnd.oci.image.manifest.v1+json) : ;;
  *) fail 'source is multi-platform index or unsupported manifest; docker tag/push would alter structure' ;;
esac
latest_json=$(inspect_json "$target")
latest_sha=$(manifest_digest "$latest_json")
latest_type=$(manifest_type "$latest_json")
printf 'IRON|DOCKERHUB_REPUSH|CURRENT_LATEST|%s|%s\n' "$target" "$latest_sha"
test "$latest_sha" = "$expected" || fail 'latest differs from expected image: new/changed release requires separate GAMP4 QA approval'
test "$latest_type" = "$source_type" || fail 'latest media type differs; manual manifest review required'
printf 'IRON|DOCKERHUB_REPUSH|EXACT_REGISTRY_STATE_CONFIRMED|latest_already_points_to_expected_digest\n'

# A same-image push is allowed, but no replacement with different content.
# Credentials must be provided through GitHub's repository secrets.
test "${DOCKERHUB_USERNAME:-}" = paulcheek || fail 'GitHub secret DOCKERHUB_USERNAME must be paulcheek'
test -n "${DOCKERHUB_TOKEN:-}" || fail 'GitHub secret DOCKERHUB_TOKEN missing: no authenticated push attempted'
export DOCKER_CONFIG="$RUNNER_TEMP/swe-hub-auth-${GITHUB_RUN_ID:-manual}"
mkdir -m 700 "$DOCKER_CONFIG" || fail 'cannot isolate Docker credentials'
cleanup() {
  docker logout docker.io >/dev/null 2>&1 || :
  rm -rf "$DOCKER_CONFIG"
}
trap cleanup EXIT
printf '%s' "$DOCKERHUB_TOKEN" | docker login --username "$DOCKERHUB_USERNAME" --password-stdin || fail 'Docker Hub login failed'
unset DOCKERHUB_TOKEN

# Download exactly the inspected registry digest to GitHub's runner only.
docker pull --platform linux/amd64 "$repo@$expected" || fail 'digest-pinned cloud pull failed'
digests=$(docker image inspect --format '{{json .RepoDigests}}' "$repo@$expected") || fail 'local source image missing'
printf '%s\n' "$digests" | jq -e --arg expected "$repo@$expected"
  'map(gsub("^(docker.io/|index.docker.io/)"; "")) | index($expected) != null' >/dev/null ||
  fail 'local source RepoDigest does not bind to expected registry manifest'

# Recheck the mutable destination immediately before attempting the push.
last_json=$(inspect_json "$target")
test "$(manifest_digest "$last_json")" = "$expected" || fail 'latest changed since preflight; abort'
docker tag "$repo@$expected" "$target" || fail 'Docker tag failed'
printf 'IRON|DOCKERHUB_REPUSH|PUSH_START|%s\n' "$target"
docker push "$target" || fail 'Docker push failed; registry needs independent inspection'
after_json=$(inspect_json "$target")
after_sha=$(manifest_digest "$after_json")
test "$after_sha" = "$expected" || fail "RELEASE_DEVIATION: pushed latest manifest became $after_sha; no successful verification"
printf 'IRON|DOCKERHUB_REPUSH|PUSH_VERIFIED|%s@%s\n' "$target" "$after_sha"
printf 'IRON|DOCKERHUB_REPUSH|SCOPE|idempotent-existing-image-only|no-new-GAMP4-release|QA_NOT_APPROVED\n'
