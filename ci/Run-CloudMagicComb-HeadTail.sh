#!/bin/sh
# SPDX-License-Identifier: 0BSD
# MagicComb HEAD + TAIL: immutable GitHub-hosted security/license research.
# All outputs are audit evidence, NOT permission to release or overwrite tags.
set -eu
export LC_ALL=C
fail() { printf 'IRON|MAGICCOMB|HOLD|%s\n' "$*" >&2; exit 1; }
test "$#" -eq 1 || fail 'expected audit or probe'
case "$1" in audit|probe) mode="$1" ;; *) fail 'unknown audit mode' ;; esac
test "${GITHUB_ACTIONS:-}" = true || fail 'GitHub Actions mandatory'
test "${RUNNER_ENVIRONMENT:-}" = github-hosted || fail 'self-hosted/user-local compute forbidden'
test "${GITHUB_REPOSITORY:-}" = Simply-Well-Executed/pentagonal-xor-cpp20 || fail 'unexpected repository'
test "${GITHUB_EVENT_NAME:-}" = push || fail 'untrusted GitHub event'
test "${GITHUB_REF:-}" = refs/heads/feat/magiccomb-license-oracle27-dotnet11-ps77-20261120 ||
  fail 'unauthorized branch'
for tool in docker git curl jq sha256sum tar grep sort awk uname; do
  command -v "$tool" >/dev/null 2>&1 || fail "missing GitHub-cloud $tool"
done
test "$(uname -m)" = x86_64 || fail 'native linux/amd64 required'
test -n "${GITHUB_SHA:-}" && test "${#GITHUB_SHA}" -eq 40 ||
  fail 'GitHub exact-source SHA absent'
test -n "${RUNNER_TEMP:-}" || fail 'GitHub isolated workdir absent'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'source root missing'
test "$(git -C "$root" rev-parse --verify HEAD)" = "$GITHUB_SHA" ||
  fail 'HEAD differs from commit event'
test -z "$(git -C "$root" status --porcelain --untracked-files=all)" ||
  fail 'dirty GitHub source checkout'
test -f "$root/ci/MagicComb-LicensePolicy.sh" || fail 'policy file absent'
test -f "$root/ci/Assert-AndroidLinux-NoPython.sh" || fail 'no-Python policy absent'
case "${DOCKER_HOST:-}" in ''|unix://*|npipe://*) : ;; *) fail 'remote Docker daemon forbidden' ;; esac
docker context inspect --format '{{.Endpoints.docker.Host}}' |
  grep -Eq '^(unix|npipe)://' || fail 'not GitHub-runner-local Docker'
docker info >/dev/null || fail 'Docker daemon unavailable'
docker buildx version >/dev/null || fail 'Docker buildx unavailable'
work="$RUNNER_TEMP/swe-magiccomb-$mode-$GITHUB_RUN_ID"
mkdir -p "$work"
printf 'IRON|MAGICCOMB|HEAD|mode=%s|source=%s|runner=github-hosted\n' "$mode" "$GITHUB_SHA"

# Download the OFFICIAL Docker Scout release binary by its GitHub API
# artifact digest (no remote install script; no Node/Python/Go source build).
scout_release=$(curl --fail-with-body --silent --show-error --retry 2 \
  -H "Authorization: Bearer $GH_TOKEN" \
  -H 'Accept: application/vnd.github+json' \
  'https://api.github.com/repos/docker/scout-cli/releases/tags/v1.29.0') ||
  fail 'official Docker Scout stable release metadata unavailable'
asset=$(printf '%s\n' "$scout_release" |
  jq -ec '.assets[] | select(.name=="docker-scout_1.29.0_linux_amd64.tar.gz")') ||
  fail 'official Scout asset unavailable'
asset_url=$(printf '%s\n' "$asset" | jq -er '.browser_download_url')
asset_digest=$(printf '%s\n' "$asset" | jq -er '.digest // empty') ||
  fail 'Scout asset has no GitHub-attested digest'
printf '%s\n' "$asset_digest" | grep -Eq '^sha256:[0-9a-f]{64}$' ||
  fail 'Scout asset SHA256 metadata invalid'
curl --fail --location --silent --show-error --retry 3 \
  "$asset_url" --output "$work/scout.tar.gz" ||
  fail 'official Scout binary asset could not be fetched'
printf '%s  %s\n' "${asset_digest#sha256:}" "$work/scout.tar.gz" |
  sha256sum -c - || fail 'Scout artifact digest does not match GitHub release metadata'
scout_entry=$(tar -tzf "$work/scout.tar.gz" |
  grep -E '(^|/)docker-scout$' | head -n 1)
test -n "$scout_entry" || fail 'no official Docker Scout executable in verified archive'
mkdir -p "$HOME/.docker/cli-plugins"
tar -xOzf "$work/scout.tar.gz" "$scout_entry" > "$HOME/.docker/cli-plugins/docker-scout"
chmod 755 "$HOME/.docker/cli-plugins/docker-scout"
docker scout version || fail 'verified Docker Scout plugin cannot execute'
printf 'IRON|MAGICCOMB|HEAD|SCOUT_ASSET_SHA256|%s\n' "$asset_digest"

# Docker Hub secrets are read-only here; no docker push/login in a builder.
if test -n "${DOCKERHUB_USERNAME:-}" && test -n "${DOCKERHUB_TOKEN:-}"; then
  export DOCKER_CONFIG="$work/docker-auth"
  mkdir -m 700 "$DOCKER_CONFIG"
  trap 'docker logout docker.io >/dev/null 2>&1 || :; rm -rf "$DOCKER_CONFIG"' EXIT
  printf '%s' "$DOCKERHUB_TOKEN" |
    docker login docker.io --username "$DOCKERHUB_USERNAME" --password-stdin ||
    fail 'Docker Hub scanner authentication failed'
  unset DOCKERHUB_TOKEN
  # Docker CLI plugin discovery uses per-user cli-plugins independent of the
  # temporary DOCKER_CONFIG. Test again to forbid silently skipping Scout.
  mkdir -p "$DOCKER_CONFIG/cli-plugins"
  cp "$HOME/.docker/cli-plugins/docker-scout" "$DOCKER_CONFIG/cli-plugins/docker-scout"
fi

if test "$mode" = audit; then
  image="paulcheek/android-source-study-v1@sha256:cf8ea2216382fd030d36caeb8092d28506c1bd74128cfb0f8beedc303aa24b62"
  docker pull --platform linux/amd64 "$image" ||
    fail 'source digest no longer accessible on Docker Hub'
  revision=$(docker image inspect --format '{{index .Config.Labels "org.opencontainers.image.revision"}}' "$image")
  test "$revision" = ed28e0544042ad700afb6e611796912294f172db ||
    fail 'candidate container source revision changed'
else
  base=mcr.microsoft.com/dotnet/sdk:11.0
  base_json=$(docker buildx imagetools inspect "$base" --format '{{json .Manifest}}') ||
    fail 'latest stable Microsoft .NET 11 image not found'
  base_digest=$(printf '%s\n' "$base_json" | jq -er '.digest // empty') ||
    fail 'no resolved .NET 11 registry SHA'
  printf '%s\n' "$base_digest" | grep -Eq '^sha256:[0-9a-fA-F]{64}$' ||
    fail 'malformed Microsoft base digest'
  printf 'IRON|MAGICCOMB|HEAD|DOTNET11_REGISTRY_DIGEST|%s\n' "$base_digest"
  base_ref="$base@$base_digest"
  docker pull --platform linux/amd64 "$base_ref" ||
    fail '.NET 11 immutable Microsoft SDK base pull failed'
  image="swe-magiccomb-oracle27-dotnet11-ps77:$GITHUB_SHA"
  docker build --no-cache --pull=false --platform linux/amd64 \
    --build-arg "DOTNET11_BASE=$base_ref" \
    --build-arg "SOURCE_REVISION=$GITHUB_SHA" \
    --file "$root/docker/MagicComb-Oracle27-DotNet11-PowerShell77.Probe.Dockerfile" \
    --tag "$image" "$root" ||
    fail 'new official Oracle Java/.NET/PowerShell toolchain build failed'
  docker run --rm --platform linux/amd64 --network none --read-only \
    --cap-drop ALL --security-opt no-new-privileges --pids-limit 512 \
    --memory 6g --cpus 2 \
    --tmpfs /tmp:rw,exec,nosuid,nodev,size=1024m,mode=1777 \
    --env HOME=/tmp --env XDG_CACHE_HOME=/tmp/.cache --env DOTNET_CLI_HOME=/tmp \
    --entrypoint /bin/sh "$image" -c '
       set -eu
       cd /study
       /bin/sh ci/Assert-AndroidLinux-NoPython.sh image
       dotnet --version
       pwsh -NoLogo -NoProfile -Command "\$PSVersionTable.PSVersion.ToString()"
       java -version
       javac -version
       cmake -S . -B /tmp/verify -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON \
         "-DCMAKE_CXX_FLAGS_RELEASE=-O2 -DNDEBUG" \
         "-DCMAKE_C_FLAGS_RELEASE=-O2 -DNDEBUG" \
         -DFETCHCONTENT_FULLY_DISCONNECTED=ON
       cmake --build /tmp/verify --parallel 2
       ctest --test-dir /tmp/verify --output-on-failure
       mkdir -p /tmp/javatest
       javac -encoding UTF-8 --release 17 -d /tmp/javatest \
         android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d/Stability.java \
         android/app/src/main/java/com/simplywellexecuted/fastestwhisper10d/Acoustic10D.java \
         android/app/src/test/java/com/simplywellexecuted/fastestwhisper10d/StandaloneTest.java
       java -cp /tmp/javatest com.simplywellexecuted.fastestwhisper10d.StandaloneTest
       /bin/sh ci/Assert-AndroidLinux-NoPython.sh image
    ' || fail 'new official toolchain CTest/Java/PowerShell runtime failed'
fi

# HEAD and TAIL both check immutable content and license without mutation.
docker run --rm --platform linux/amd64 --network none --read-only \
  --cap-drop ALL --security-opt no-new-privileges --pids-limit 128 \
  --entrypoint /bin/sh "$image" /study/ci/Assert-AndroidLinux-NoPython.sh image ||
  fail 'candidate lost its existing Python-free guarantee'

# License audit intentionally returns HOLD until a complete owner-approved
# ledger exists. Continue vulnerability discovery so both reasons are visible.
if docker run --rm --platform linux/amd64 --network none --read-only \
  --cap-drop ALL --security-opt no-new-privileges --pids-limit 128 \
  --mount "type=bind,src=$root/ci/MagicComb-LicensePolicy.sh,dst=/magiccomb-license-policy.sh,readonly" \
  --entrypoint /bin/sh "$image" /magiccomb-license-policy.sh image \
  > "$work/license-report.log" 2>&1; then
  license_result=PASS
else
  license_result=EXCEPTION_REQUIRED
fi
printf 'IRON|MAGICCOMB|LICENSE|RESULT|%s\n' "$license_result"
grep -E '^IRON\|MAGICCOMB\|LICENSE\|(HEAD|FORBIDDEN|UNKNOWN|HOLD|TAIL)' \
  "$work/license-report.log" | head -n 70 || :
sha256sum "$work/license-report.log" |
  awk '{printf "IRON|MAGICCOMB|LICENSE|REPORT_SHA256|%s\n",$1}'
license_forbidden=$(grep -c '^IRON|MAGICCOMB|LICENSE|FORBIDDEN|' "$work/license-report.log" || :)
license_unknown=$(grep -c '^IRON|MAGICCOMB|LICENSE|UNKNOWN|' "$work/license-report.log" || :)
printf 'IRON|MAGICCOMB|LICENSE|COUNTS|forbidden=%s|unknown=%s\n' "$license_forbidden" "$license_unknown"

# Official Docker Scout scan. The report is NOT suppressed or claimed zero.
if docker scout cves --format markdown "$image" > "$work/scout.md" 2>&1; then
  scout_result=OK
else
  scout_result=SCAN_ERROR
fi
if test "$scout_result" = OK; then
  printf 'IRON|MAGICCOMB|SCOUT|SUMMARY\n'
  grep -Eo 'https://img.shields.io/badge/(critical|high|medium|low|unspecified)-[0-9]+' \
    "$work/scout.md" | head -n 10 || :
  printf 'IRON|MAGICCOMB|SCOUT|QUICKVIEW\n'
  if docker scout quickview "$image" > "$work/scout-quickview.log" 2>&1; then
    tail -n 65 "$work/scout-quickview.log"
  else
    printf 'IRON|MAGICCOMB|SCOUT|QUICKVIEW_UNAVAILABLE\n'
  fi
  printf 'IRON|MAGICCOMB|SCOUT|SAMPLE_CVES\n'
  grep -E 'CVE-[0-9]{4}-[0-9]+' "$work/scout.md" | head -n 26 || :
else
  printf 'IRON|MAGICCOMB|SCOUT|HOLD|scanner could not complete, cannot claim zero CVEs\n'
  tail -n 50 "$work/scout.md" || :
fi
sha256sum "$work/scout.md" |
  awk '{printf "IRON|MAGICCOMB|SCOUT|REPORT_SHA256|%s\n",$1}'

if test "$mode" = audit; then
  newest=$(docker buildx imagetools inspect \
    'paulcheek/android-source-study-v1:python-free-candidate-ed28e0544042' \
    --format '{{json .Manifest}}' | jq -er '.digest // empty')
  test "$newest" = sha256:cf8ea2216382fd030d36caeb8092d28506c1bd74128cfb0f8beedc303aa24b62 ||
    fail 'MagicComb TAIL detected changed candidate digest'
fi
printf 'IRON|MAGICCOMB|TAIL|mode=%s|source=%s|license=%s|scout=%s|no_registry_push\n' \
  "$mode" "$GITHUB_SHA" "$license_result" "$scout_result"
fail 'user exception and source-matched zero-vulnerability receipt required BEFORE release'
