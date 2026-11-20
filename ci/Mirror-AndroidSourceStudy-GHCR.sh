#!/bin/sh
# SPDX-License-Identifier: 0BSD
# GitHub-hosted only. Mirror the historical image byte-for-byte into GHCR.
# This is NOT a claim that the historical digest is Python-free.
set -eu
export LC_ALL=C
fail() { printf 'IRON|GHCR_MIRROR|HOLD|%s\n' "$*" >&2; exit 1; }
test "${GITHUB_ACTIONS:-}" = true || fail 'GitHub Actions mandatory'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'self-hosted/local compute prohibited'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 || fail 'repository mismatch'
test "${GITHUB_REF:-}" = refs/heads/feat/github-package-source-study-20261120 || fail 'unexpected branch'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'only committed branch source can launch mirror'
test -n "${GITHUB_SHA:-}" && test -n "${RUNNER_TEMP:-}" || fail 'source/run provenance missing'
test "$(uname -m)" = x86_64 || fail 'native Linux amd64 runner required'
for tool in docker git grep curl jq; do command -v "$tool" >/dev/null 2>&1 || fail "missing runner tool $tool"; done
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker endpoint forbidden' ;; esac
docker context inspect --format '{{.Endpoints.docker.Host}}' | grep -Eq '^(unix|npipe)://' ||
  fail 'Docker not local to cloud runner'
docker info >/dev/null || fail 'cloud Docker unavailable'
docker buildx version >/dev/null || fail 'Docker buildx unavailable'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'checked-out root unavailable'
test "$(git -C "$root" rev-parse HEAD)" = "$GITHUB_SHA" || fail 'source SHA mismatch'
test -z "$(git -C "$root" status --porcelain --untracked-files=all)" || fail 'dirty source tree'

source_tag=paulcheek/android-source-study-v1:sha-07eb2a4561f0
source_digest=sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30
ghcr=ghcr.io/simply-well-executed/android-source-study-v1
tagged="$ghcr:sha-07eb2a4561f0"
manifest=$(docker buildx imagetools inspect "$source_tag" --format '{{json .Manifest}}') ||
  fail 'Docker Hub source tag unreadable'
source_actual=$(printf '%s\n' "$manifest" | jq -er '.digest // empty') ||
  fail 'Docker Hub source manifest digest absent'
source_type=$(printf '%s\n' "$manifest" | jq -er '.mediaType // empty') ||
  fail 'Docker Hub source manifest type absent'
test "$source_actual" = "$source_digest" || fail "historical source digest deviation: $source_actual"
case "$source_type" in
  application/vnd.docker.distribution.manifest.v2+json|application/vnd.oci.image.manifest.v1+json) : ;;
  *) fail 'source image is an index; require explicit multiarch review' ;;
esac
# Verify the embedded source-repository label before GitHub auto-association.
docker pull --platform linux/amd64 "paulcheek/android-source-study-v1@$source_digest" ||
  fail 'digest-pinned public source pull rejected'
label=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.source"}}' "paulcheek/android-source-study-v1@$source_digest") ||
  fail 'OCI provenance label not readable'
test "$label" = https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20 ||
  fail "source package label does not connect intended GitHub repository: $label"
test -n "${GHCR_TOKEN:-}" || fail 'no GitHub package-write token'
export DOCKER_CONFIG="$RUNNER_TEMP/swe-ghcr-auth-$GITHUB_RUN_ID"
mkdir -m 700 "$DOCKER_CONFIG"
trap 'docker logout ghcr.io >/dev/null 2>&1 || :; rm -rf "$DOCKER_CONFIG"' EXIT
printf '%s' "$GHCR_TOKEN" | docker login ghcr.io -u "$GITHUB_ACTOR" --password-stdin ||
  fail 'GHCR login with repository token rejected'
# This is a registry-to-registry carbon copy: no Dockerfile rebuild,
# no altered layers, no new source claims and no mutable generic latest tag.
docker buildx imagetools create --prefer-index=false --tag "$tagged" \
  "paulcheek/android-source-study-v1@$source_digest" ||
  fail 'GHCR registry-to-registry copy failed'
after=$(docker buildx imagetools inspect "$tagged" --format '{{json .Manifest}}') ||
  fail 'GHCR published manifest is unreadable'
ghcr_digest=$(printf '%s\n' "$after" | jq -er '.digest // empty') ||
  fail 'GHCR digest missing'
test "$ghcr_digest" = "$source_digest" ||
  fail "GHCR manifest digest differs from Docker Hub (no identical mirror claim): $ghcr_digest"
printf 'IRON|GHCR_MIRROR|DIGEST_VERIFIED|%s@%s\n' "$tagged" "$ghcr_digest"
# GHCR packages are initially private by default, even if a repo is public.
# Record actual package visibility. Do not claim public visibility without proof.
package_info=$(curl --fail-with-body --silent --show-error --retry 2 \
  -H "Authorization: Bearer $GHCR_TOKEN" \
  -H 'Accept: application/vnd.github+json' \
  -H 'X-GitHub-Api-Version: 2022-11-28' \
  "https://api.github.com/orgs/Simply-Well-Executed/packages/container/android-source-study-v1") ||
  fail 'GitHub Packages API readback failed'
visibility=$(printf '%s\n' "$package_info" | jq -r '.visibility // "unknown"')
url=$(printf '%s\n' "$package_info" | jq -r '.html_url // "unavailable"')
printf 'IRON|GHCR_MIRROR|PACKAGE_VISIBILITY|%s|%s\n' "$visibility" "$url"
printf 'IRON|GHCR_MIRROR|HISTORICAL_NOT_PYTHON_FREE_CLAIM|QA_NOT_APPROVED|NO_NEW_BUILD\n'
