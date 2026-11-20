#!/bin/sh
# SPDX-License-Identifier: 0BSD
# GitHub-hosted-only candidate publication: GHCR -> Docker Hub.
# Promote the exact tested single-platform image. No Python, Node,
# local compute, rebuild, mutable :latest replacement or QA bypass.
set -eu
export LC_ALL=C

fail() { printf 'IRON|DOCKERHUB_PYTHONFREE|HOLD|%s\n' "$*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || fail "missing GitHub cloud tool $1"; }
for program in git docker curl jq grep uname mktemp; do need "$program"; done
test "${GITHUB_ACTIONS:-}" = true || fail 'must execute inside GitHub Actions'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'self-hosted and local compute forbidden'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 ||
  fail 'unrecognized repository'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'unexpected GitHub event'
test "${GITHUB_REF:-}" = refs/heads/feat/dockerhub-pythonfree-candidate-deployment-20261120 ||
  fail 'unrecognized deployment-authorization branch'
test "$(uname -m)" = x86_64 || fail 'requires real Linux amd64 GitHub hosted runner'
test -n "${GITHUB_SHA:-}" && test "${#GITHUB_SHA}" -eq 40 || fail 'GitHub event SHA missing'
test -n "${RUNNER_TEMP:-}" && test -n "${GITHUB_RUN_ID:-}" ||
  fail 'GitHub run custody missing'
test "${DOCKERHUB_USERNAME:-}" = paulcheek || fail 'approved Docker Hub namespace mismatch'
test -n "${DOCKERHUB_TOKEN:-}" || fail 'Docker Hub GitHub secret not configured'
test -n "${GHCR_TOKEN:-}" || fail 'GitHub package token not configured'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon not allowed' ;; esac
docker context inspect --format '{{.Endpoints.docker.Host}}' |
  grep -Eq '^(unix|npipe)://' || fail 'Docker engine not local to GitHub hosted runner'
docker info >/dev/null || fail 'GitHub cloud Docker engine unavailable'
docker buildx version >/dev/null || fail 'Docker buildx unavailable'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source checkout unavailable'
test "$(git -C "$root" rev-parse HEAD)" = "$GITHUB_SHA" || fail 'unverified workflow source SHA'
test -z "$(git -C "$root" status --porcelain --untracked-files=all)" ||
  fail 'dirty source checkout not authorized'

# These are TWO INDEPENDENT release identities. Never change old :latest.
source_sha=ed28e0544042ad700afb6e611796912294f172db
source_repo=ghcr.io/simply-well-executed/android-source-study-v1
source_tag="$source_repo:python-free-candidate-ed28e0544042"
source_digest=sha256:cf8ea2216382fd030d36caeb8092d28506c1bd74128cfb0f8beedc303aa24b62
dockerhub_repo=paulcheek/android-source-study-v1
candidate_tag=python-free-candidate-ed28e0544042
destination="$dockerhub_repo:$candidate_tag"
old_latest=sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30

# Do not trust an asserted test result: read actual GitHub commit status for
# the exact source revision that built the candidate image on cloud compute.
test_status=$(curl --fail-with-body --silent --show-error --retry 2 \
  -H "Authorization: Bearer $GHCR_TOKEN" \
  -H 'Accept: application/vnd.github+json' \
  -H 'X-GitHub-Api-Version: 2022-11-28' \
  "https://api.github.com/repos/$GITHUB_REPOSITORY/commits/$source_sha/status" |
  jq -er '[.statuses[] | select(.context=="swe/ghcr-python-free-android-linux-candidate")][0].state // empty') ||
  fail 'source-matched GHCR CTest/Java/Python-free commit status cannot be verified'
test "$test_status" = success || fail "source validation is not success: $test_status"
printf 'IRON|DOCKERHUB_PYTHONFREE|SOURCE_GITHUB_VALIDATION|PASS|%s\n' "$source_sha"

export DOCKER_CONFIG="$RUNNER_TEMP/swe-candidate-publish-$GITHUB_RUN_ID"
mkdir -m 700 "$DOCKER_CONFIG" || fail 'cannot isolate registry credentials'
cleanup() {
  docker logout docker.io >/dev/null 2>&1 || :
  docker logout ghcr.io >/dev/null 2>&1 || :
  rm -rf "$DOCKER_CONFIG"
}
trap cleanup EXIT
printf '%s' "$GHCR_TOKEN" |
  docker login ghcr.io --username "$GITHUB_ACTOR" --password-stdin ||
  fail 'GHCR private-package login failed'
printf '%s' "$DOCKERHUB_TOKEN" |
  docker login docker.io --username "$DOCKERHUB_USERNAME" --password-stdin ||
  fail 'Docker Hub authenticated login failed'
unset DOCKERHUB_TOKEN

manifest_digest() {
  printf '%s\n' "$1" | jq -er '.digest // empty' || fail 'registry manifest lacks digest'
}
manifest_type() {
  printf '%s\n' "$1" | jq -er '.mediaType // empty' || fail 'registry manifest lacks media type'
}
inspect() {
  docker buildx imagetools inspect "$1" --format '{{json .Manifest}}' ||
    fail "unable to read registry manifest: $1"
}
source_json=$(inspect "$source_tag")
test "$(manifest_digest "$source_json")" = "$source_digest" ||
  fail 'GHCR tag no longer points to the original tested candidate manifest'
source_type=$(manifest_type "$source_json")
case "$source_type" in
  application/vnd.docker.distribution.manifest.v2+json|application/vnd.oci.image.manifest.v1+json) : ;;
  *) fail 'GHCR candidate is an unreviewed manifest index/multiplatform image' ;;
esac
printf 'IRON|DOCKERHUB_PYTHONFREE|GHCR_IMMUTABLE_SOURCE|%s@%s\n' "$source_repo" "$source_digest"

# Prove source-revision link and no Python before cross-registry promotion.
docker pull --platform linux/amd64 "$source_repo@$source_digest" ||
  fail 'cannot pull digest-pinned GHCR source'
revision=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.revision"}}' "$source_repo@$source_digest") ||
  fail 'source image revision label missing'
origin=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.source"}}' "$source_repo@$source_digest") ||
  fail 'source repository label missing'
test "$revision" = "$source_sha" || fail 'source container revision differs from passing CTest SHA'
test "$origin" = "https://github.com/$GITHUB_REPOSITORY" ||
  fail 'source image ownership/provenance label mismatch'
docker run --rm --platform linux/amd64 --network none --read-only \
  --cap-drop ALL --security-opt no-new-privileges --pids-limit 128 \
  --entrypoint /bin/sh "$source_repo@$source_digest" \
  /study/ci/Assert-AndroidLinux-NoPython.sh image ||
  fail 'source digest contains forbidden Python executable/package'

# Protect the historical Docker Hub latest tag and reject overwriting
# an existing *different* candidate image.
latest_before=$(manifest_digest "$(inspect "$dockerhub_repo:latest")")
test "$latest_before" = "$old_latest" ||
  fail 'historical Docker Hub latest changed: require separate release review'
status_code=$(curl --silent --show-error --location \
  --output "$RUNNER_TEMP/dockerhub-candidate-preflight.json" \
  --write-out '%{http_code}' \
  "https://hub.docker.com/v2/repositories/paulcheek/android-source-study-v1/tags/$candidate_tag") ||
  fail 'Docker Hub candidate tag existence preflight unavailable'
case "$status_code" in
  404) printf 'IRON|DOCKERHUB_PYTHONFREE|NEW_TAG|%s\n' "$candidate_tag" ;;
  200)
    existing=$(manifest_digest "$(inspect "$destination")")
    test "$existing" = "$source_digest" ||
      fail "candidate tag already exists with a DIFFERENT digest: $existing"
    printf 'IRON|DOCKERHUB_PYTHONFREE|ALREADY_SAME_DIGEST|%s\n' "$candidate_tag"
    ;;
  *) fail "Docker Hub tag API returned HTTP $status_code; refuse overwrite" ;;
esac

# Preserve the single source manifest instead of wrapping it in a new index.
# No Dockerfile build, source mutation, :latest retag or Node/Python action.
printf 'IRON|DOCKERHUB_PYTHONFREE|PUSH_START|%s\n' "$destination"
docker buildx imagetools create --prefer-index=false --tag "$destination" \
  "$source_repo@$source_digest" ||
  fail 'registry-to-registry Docker Hub candidate publication failed'

# Registry readback must match the original approved source digest exactly.
dest_json=$(inspect "$destination")
dest_digest=$(manifest_digest "$dest_json")
test "$dest_digest" = "$source_digest" ||
  fail "RELEASE_DEVIATION: Docker Hub candidate digest $dest_digest differs from GHCR"
test "$(manifest_type "$dest_json")" = "$source_type" ||
  fail 'destination manifest format changed during registry transfer'
test "$(manifest_digest "$(inspect "$dockerhub_repo:latest")")" = "$old_latest" ||
  fail 'historical latest moved during candidate publication'
docker pull --platform linux/amd64 "$dockerhub_repo@$dest_digest" ||
  fail 'Docker Hub immutable candidate digest could not be pulled'
docker run --rm --platform linux/amd64 --network none --read-only \
  --cap-drop ALL --security-opt no-new-privileges --pids-limit 128 \
  --entrypoint /bin/sh "$dockerhub_repo@$dest_digest" \
  /study/ci/Assert-AndroidLinux-NoPython.sh image ||
  fail 'independently pulled Docker Hub candidate failed Python prohibition'
printf 'IRON|DOCKERHUB_PYTHONFREE|PUSH_VERIFIED|%s@%s\n' "$destination" "$dest_digest"
printf 'IRON|DOCKERHUB_PYTHONFREE|RELEASE_BOUNDARY|candidate|linux-amd64|QA_NOT_APPROVED|ANDROID_DEVICE_NOT_TESTED|LATEST_UNCHANGED\n'
