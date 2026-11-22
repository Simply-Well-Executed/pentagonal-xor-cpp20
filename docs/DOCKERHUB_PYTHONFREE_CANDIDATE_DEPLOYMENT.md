# Docker Hub Python-free Android/Linux candidate: verified registry deployment

**Historical shell instructions:** POSIX commands and shell behavior described below are preserved as historical protocol/evidence. The 17 wrappers are removed from active CI; links target archived text. See the [discarded-files registry](../discardedfiles.md). Retained task names invoke the native fixed transcript and exit 42 / **RELEASE=HOLD**, without performing these former operations.

**Status:** SOURCE-MATCHED CANDIDATE PUBLISHED TO DOCKER HUB; GITHUB DEPLOYMENT SUCCESS; NOT PRODUCTION QUALIFIED.

## Public candidate location

- Docker Hub repository: `paulcheek/android-source-study-v1`.
- **New candidate tag:** `python-free-candidate-ed28e0544042`.
- [Docker Hub tag listing](https://hub.docker.com/r/paulcheek/android-source-study-v1/tags?name=python-free-candidate-ed28e0544042). The general repository page is not a per-tag deployment timeline. Use the **Tags** page for the newly published image.
- Complete registry digest (identical in GitHub Container Registry and Docker Hub): `sha256:cf8ea2216382fd030d36caeb8092d28506c1bd74128cfb0f8beedc303aa24b62`.
- GitHub Container Registry source: `ghcr.io/simply-well-executed/android-source-study-v1:python-free-candidate-ed28e0544042`.

## Exact source, testing and real publication

1. **Builder source commit:** `ed28e0544042ad700afb6e611796912294f172db`. GitHub cloud [run #44753219140](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44753219140) built the Python-free Android/Linux SDK-free candidate using explicit `-O2`, passed **45/45 native C++20 CTests** and **26,654 standalone Java 17 checks**. Image interpreter/package exclusion passed after GHCR publication and digest pull. Exact GitHub status `swe/ghcr-python-free-android-linux-candidate = success` was checked again before Docker Hub publication.
2. **Real Docker Hub candidate push:** GitHub-hosted [Actions run #44758095928](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44758095928) at workflow commit `831c0c1f9a54b64c86b07acdb578d7c4e247034f` verified the immutable GHCR manifest, source label and original commit status; authenticated to both registries; created the new Docker Hub candidate tag without rebuilding or repackaging; independently pulled its Docker Hub digest and rechecked no Python.
3. **GitHub Deployment record:** [repository Deployments](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/deployments), environment `dockerhub-python-free-candidate`, deployment ID `7944706336`. A separate GitHub-hosted audit job fetched the deployment and most recent status using the GitHub REST API and returned **`success`**, confirmed by commit status `swe/github-dockerhub-candidate-deployment = success`.
4. The publisher's exact commit status `swe/dockerhub-pythonfree-candidate-publish = success` links back to the action log. Source, destination and release-hold identities remain distinct in the audit.

## Unchanged historic release and security constraints

- Docker Hub `paulcheek/android-source-study-v1:latest` still resolves to the **historic** manifest `sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30`. The new candidate has a separate tag and digest; the publisher verifies old `latest` both before and after its operation.
- This is **Linux amd64 image distribution**, not device/OS deployment. The `dockerhub-python-free-candidate` GitHub deployment environment indicates availability of an image in Docker Hub. It does not claim an Android application is running, nor that the deployment represents QA release approval.
- Independent 13-profile GAMP4 testing, Android device OQ, QA signoff, SBOM/license approval and production promotion are not implied. Do not rename this candidate to `latest` under the existing release policy.
- GitHub-hosted runners only; self-hosted/local machines prohibited. The authored workflows use Docker, Git, POSIX shell, curl and jq, with no Node/npm or Python commands or third-party marketplace actions. No proprietary image credentials are committed to Git.
- The new candidate is checked against the accepted no-Python source/runtime gate, with non-root and offline runtime validation. A hidden embedded interpreter in arbitrary opaque binaries is not disproven by this source/package/executable inventory.

## Repository references

- [Python-free Android/Linux policy](ANDROID_LINUX_NO_PYTHON.md).
- [Cloud candidate publishing script](../Archive/Retired/ci/Promote-VerifiedAndroidLinux-ToDockerHub.sh.txt).
- [Cloud deployment workflow](../.github/workflows/dockerhub-pythonfree-candidate-deployment.yml).
- [Earlier historic Docker Hub same-digest publication](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44746338664).
