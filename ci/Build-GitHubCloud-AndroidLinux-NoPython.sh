#!/bin/sh
# SPDX-License-Identifier: 0BSD
# Candidate only: build the Android/Linux source stage without Python.
# Real GitHub-hosted test and GHCR push. No full 13-profile release claim.
set -eu
export LC_ALL=C
fail() { printf 'IRON|ANDROID_LINUX_NO_PYTHON|HOLD|%s\n' "$*" >&2; exit 1; }
test "${GITHUB_ACTIONS:-}" = true || fail 'GitHub cloud required'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'no self-hosted/local compute'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 || fail 'unexpected repository'
test "${GITHUB_REF:-}" = refs/heads/feat/github-package-source-study-20261120 || fail 'unexpected source branch'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'untrusted event'
test -n "${GITHUB_SHA:-}" && test "${#GITHUB_SHA}" -eq 40 || fail 'missing exact Git SHA'
test -n "${GHCR_TOKEN:-}" || fail 'GitHub package token absent'
test -n "${RUNNER_TEMP:-}" || fail 'GitHub temp root unavailable'
for tool in git docker jq grep uname; do command -v "$tool" >/dev/null 2>&1 || fail "missing $tool"; done
test "$(uname -m)" = x86_64 || fail 'Linux amd64 required'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker socket rejected' ;; esac
docker context inspect --format '{{.Endpoints.docker.Host}}' | grep -Eq '^(unix|npipe)://' ||
  fail 'Docker daemon not local to hosted runner'
docker info >/dev/null || fail 'GitHub Docker unavailable'
docker buildx version >/dev/null || fail 'Buildx missing'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'Git source absent'
cd "$root" || fail 'cannot enter source root'
test "$(git rev-parse HEAD)" = "$GITHUB_SHA" || fail 'checked-out commit not event commit'
test -z "$(git status --porcelain --untracked-files=all)" || fail 'dirty source checkout'
/bin/sh ci/Assert-AndroidLinux-NoPython.sh source || fail 'Python-tainted tracked source'

base=mcr.microsoft.com/dotnet/sdk:10.0-noble
base_json=$(docker buildx imagetools inspect "$base" --format '{{json .Manifest}}') ||
  fail 'cannot inspect Microsoft base registry manifest'
base_digest=$(printf '%s\n' "$base_json" | jq -er '.digest // empty') ||
  fail 'Microsoft base registry manifest lacks digest'
printf '%s\n' "$base_digest" | grep -Eq '^sha256:[0-9a-fA-F]{64}$' ||
  fail 'base registry digest is not sha256'
pinned_base="$base@$base_digest"
printf 'IRON|ANDROID_LINUX_NO_PYTHON|PINNED_MCR_BASE|%s\n' "$pinned_base"
docker pull --platform linux/amd64 "$pinned_base" ||
  fail 'immutable Microsoft SDK base image unavailable'
local_image="swe-android-linux-python-free:$GITHUB_SHA"
docker build --platform linux/amd64 --pull=false --no-cache \
  --target source \
  --build-arg "DOTNET_SDK_BASE=$pinned_base" \
  --build-arg "SOURCE_REVISION=$GITHUB_SHA" \
  --file docker/android-source-study-v1.Dockerfile \
  --tag "$local_image" . ||
  fail 'Python-free Android/Linux source stage did not build'

image_id=$(docker image inspect --format '{{.Id}}' "$local_image") ||
  fail 'built source image unavailable'
label=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.source"}}' "$local_image")
revision=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.revision"}}' "$local_image")
test "$label" = https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20 ||
  fail 'wrong GitHub source-provenance label'
test "$revision" = "$GITHUB_SHA" || fail 'unbound source revision'
printf 'IRON|ANDROID_LINUX_NO_PYTHON|BUILD_IMAGE_CONFIG|%s\n' "$image_id"

# Test exact published candidate bytes inside non-root, offline container.
# Use /tmp for generated files; /study source and image root are read-only.
docker run --rm --platform linux/amd64 \
  --network none --read-only --cap-drop ALL \
  --security-opt no-new-privileges --pids-limit 512 --memory 6g --cpus 2 \
  --tmpfs /tmp:rw,nosuid,nodev,size=1024m \
  --entrypoint /bin/sh "$local_image" -c '
    set -eu
    cd /study
    /bin/sh ci/Assert-AndroidLinux-NoPython.sh image
    cmake -S . -B /tmp/swe-release -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTING=ON -DCMAKE_CXX_STANDARD=20 \
      "-DCMAKE_CXX_FLAGS_RELEASE=-O2 -DNDEBUG" \
      "-DCMAKE_C_FLAGS_RELEASE=-O2 -DNDEBUG" \
      -DFETCHCONTENT_FULLY_DISCONNECTED=ON
    cmake --build /tmp/swe-release --config Release --parallel 2
    ctest --test-dir /tmp/swe-release -C Release --output-on-failure
    mkdir -p /tmp/swe-java
    javac -encoding UTF-8 --release 17 -d /tmp/swe-java \
      android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d/Stability.java \
      android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d/Acoustic10D.java \
      android/app/src/test/java/com/simplywellexecuted/fastestwhisper10d/StandaloneTest.java
    java -cp /tmp/swe-java com.simplywellexecuted.fastestwhisper10d.StandaloneTest
    /bin/sh ci/Assert-AndroidLinux-NoPython.sh image
    echo IRON\|ANDROID_LINUX_NO_PYTHON\|PASS\|CTest_Java_NoPython
  ' || fail 'runtime CMake/CTest/Java/Python-free audit failed'

# The new candidate is NOT the historical Docker Hub image.
# Publish under a clearly identified, immutable-intent candidate label only.
short=$(printf '%s' "$GITHUB_SHA" | cut -c1-12)
target="ghcr.io/simply-well-executed/android-source-study-v1:python-free-candidate-$short"
export DOCKER_CONFIG="$RUNNER_TEMP/swe-python-free-ghcr-$GITHUB_RUN_ID"
mkdir -m 700 "$DOCKER_CONFIG"
trap 'docker logout ghcr.io >/dev/null 2>&1 || :; rm -rf "$DOCKER_CONFIG"' EXIT
printf '%s' "$GHCR_TOKEN" | docker login ghcr.io -u "$GITHUB_ACTOR" --password-stdin ||
  fail 'GitHub package login rejected'
docker tag "$local_image" "$target" || fail 'candidate tag failed'
docker push "$target" || fail 'candidate GHCR push failed'
remote=$(docker buildx imagetools inspect "$target" --format '{{json .Manifest}}') ||
  fail 'cannot read candidate GHCR manifest after push'
remote_digest=$(printf '%s\n' "$remote" | jq -er '.digest // empty') ||
  fail 'published candidate manifest SHA missing'
printf '%s\n' "$remote_digest" | grep -Eq '^sha256:[0-9a-fA-F]{64}$' ||
  fail 'published candidate manifest not sha256'
# Independent digest pull and second no-Python runtime check.
docker pull --platform linux/amd64 "ghcr.io/simply-well-executed/android-source-study-v1@$remote_digest" ||
  fail 'cannot pull published GHCR digest'
docker run --rm --platform linux/amd64 --network none --read-only \
  --cap-drop ALL --security-opt no-new-privileges \
  --entrypoint /bin/sh "ghcr.io/simply-well-executed/android-source-study-v1@$remote_digest" \
  /study/ci/Assert-AndroidLinux-NoPython.sh image ||
  fail 'published immutable candidate no-Python check failed'
printf 'IRON|ANDROID_LINUX_NO_PYTHON|GHCR_CANDIDATE_VERIFIED|%s@%s\n' "$target" "$remote_digest"
printf 'IRON|ANDROID_LINUX_NO_PYTHON|SCOPE|candidate_only|GAMP4_QA_NOT_APPROVED|ANDROID_DEVICE_NOT_TESTED\n'
