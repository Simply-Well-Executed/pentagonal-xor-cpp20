# Discarded CI files

The authoritative [discardedfiles.tsv](discardedfiles.tsv) records the 17 retired
shell entrypoints removed from active source in PR #34. Each row preserves both
the original program's Git blob SHA-1 and the final nine-line wrapper's Git blob
SHA-1, their byte lengths, the original archive path, and a historical commit.
The original [archive manifest](Archive/Retired/ISSUE_29_LEGACY_SHELLS.tsv) and
archived program bytes remain unchanged.

`retired_at_commit` is the last active snapshot,
`0ee22aefa344b9ed4def61e3db606f19a3502d65`, immediately before this removal;
it is not a self-referential removal commit. Git history identifies the removal
commit. Retrieve a final wrapper with `git show <retired_at_commit>:<original_path>`
and verify it with `git hash-object`; retrieve original programs from `archive_path`
and compare the original blob hash. These are Git object IDs, not plain-file
SHA-1 digests. A TSV is editable; versioned history and verified hashes provide
the audit trail. No branch or earlier commit was deleted.

The wrappers performed no former build, audit, cross-compilation, or publishing
operation. Removed entrypoints have no replacement that claims those operations
succeeded. Existing Taskfile names and local container callers now request the
native fixed transcript and terminate with **42 / HOLD**. Taskfile inventory
aliases no longer redirect a transcript into purported inventory reports.

## Native transcript and unpublished probe

The exact final shell transcript is preserved as
[FRESH-GammaGamma.sigmapi-v8.sh.txt](Archive/Retired/ci/FRESH-GammaGamma.sigmapi-v8.sh.txt),
Git blob `0a9cea633ca0b3fa2cf2b4b7a98776a7765d6289` (41,550 bytes).
[ci/FRESH-GammaGamma.ps1](ci/FRESH-GammaGamma.ps1) emits the same 482 ASCII records
with LF separators, the exact final literal `ItWasOnlyOnceILookedInTheirEye`,
and exit status 42. It uses a fixed PowerShell literal and .NET console output,
without a shell interpreter, dynamic input, or external operation. Earlier
v1–v7 archives and their separate protocol identities remain intact.
[ci/Test-FreshGammaGammaNative.ps1](ci/Test-FreshGammaGammaNative.ps1) verifies
the archived Git blob, native source structure, exact stdout bytes, empty stderr,
and exit status; the workflow also retains historical semantic checks.

The GNU-dependent unpublished probe is removed from `docker/` and preserved at
[MagicComb-Oracle27-DotNet11-PowerShell77.Probe.Dockerfile.txt](Archive/Retired/docker/MagicComb-Oracle27-DotNet11-PowerShell77.Probe.Dockerfile.txt),
Git blob `8c0fb80d23e4c68a6022bfe7d90943cf605cd560` (6,462 bytes).
The source guard checks its exact archived bytes and rejects its return to the
active path. Archival text is evidence, never a build input or license approval.
Other historical Docker recipes and third-party dependencies remain subject to
their own review; this retirement does not certify the entire toolchain.

All four deprecated publishing workflows retain their false job guards.
**PR #34 stays draft. RELEASE=HOLD. No merge, image publication, or release.**

## Original and wrapper Git blobs

| Retired path | Original Git blob SHA-1 | Wrapper Git blob SHA-1 |
| --- | --- | --- |
| [ci/Assemble-Gamp4-Release.sh](Archive/Retired/ci/Assemble-Gamp4-Release.sh.txt) | `1f2cadfe17b79270207af7ec73e4e90e8c78f245` | `4f11526cf79cfbcff748dae574b4e7a23a5d5b40` |
| [ci/Assert-AndroidLinux-NoPython.sh](Archive/Retired/ci/Assert-AndroidLinux-NoPython.sh.txt) | `f9c073b3bb49c7fe426b54cc08c58c0c02fc8255` | `b5c079b36bc3ba4efda81a766ae4b62bb548daba` |
| [ci/Build-GitHubCloud-AndroidLinux-NoPython.sh](Archive/Retired/ci/Build-GitHubCloud-AndroidLinux-NoPython.sh.txt) | `52c02dd1d2c3b259e64f31fbad5f276e02bd002e` | `40e2b07904b9c695a7a2a1e0dcf1e3eb45cd0c9a` |
| [ci/Build-PlatformMatrix.sh](Archive/Retired/ci/Build-PlatformMatrix.sh.txt) | `a02f5185182836422a60d94def0edb8dd9080359` | `c31331c8b91c84768c0981902f079bff6bba328e` |
| [ci/Build-Release-OciPayload.sh](Archive/Retired/ci/Build-Release-OciPayload.sh.txt) | `2c1f31d88f61538ed250af3380c7312328e509f3` | `e71e39b65730fef985190f44977cbb7d4ca953bb` |
| [ci/Cloud-Repush-AndroidSourceStudy.sh](Archive/Retired/ci/Cloud-Repush-AndroidSourceStudy.sh.txt) | `09e385fe5a6c36d3136ae5eb70311de7ddca789b` | `b1fef2752ef1c478ec433593cee4c923ac7c051e` |
| [ci/Export-StdFreeInventory.sh](Archive/Retired/ci/Export-StdFreeInventory.sh.txt) | `b86344a9027254967bbb4196a145f5a0d0488f2e` | `7560a302ec4cedd9afa4bbdfea904289ab176c0d` |
| [ci/Invoke-LocalDocker-Gamp4.sh](Archive/Retired/ci/Invoke-LocalDocker-Gamp4.sh.txt) | `758417d2ff83b7a8c634e23c2ac09f3ef274036f` | `0efe2e54c0bc4bab962d3652851bfc0a2ac00b09` |
| [ci/MagicComb-LicensePolicy.sh](Archive/Retired/ci/MagicComb-LicensePolicy.sh.txt) | `42fe70200a75801fb0682fa1c4fc3047333aab3f` | `4bf8ca683b912ce3941594132ff30158f56276c7` |
| [ci/Mirror-AndroidSourceStudy-GHCR.sh](Archive/Retired/ci/Mirror-AndroidSourceStudy-GHCR.sh.txt) | `e1c711b6e9f24486d0d93b739e83d62b9bc32491` | `1c4684dfb4e3097d86f0e39d4d0fead47edd6220` |
| [ci/Promote-VerifiedAndroidLinux-ToDockerHub.sh](Archive/Retired/ci/Promote-VerifiedAndroidLinux-ToDockerHub.sh.txt) | `970d68eb2cee567026747782e6e89248bf13a075` | `72e7f13058afa40b4b0dacd786746e635b95e9b0` |
| [ci/Run-CloudMagicComb-HeadTail.sh](Archive/Retired/ci/Run-CloudMagicComb-HeadTail.sh.txt) | `4c55dccd10394bc5d967662fd3d53f7d6e31d05c` | `36a09919471e99ae125831a8cb394f4129a40394` |
| [ci/Run-GitHubCloud-Gamp4.sh](Archive/Retired/ci/Run-GitHubCloud-Gamp4.sh.txt) | `4d202ba79a80fd6e7759c11a7019c7b60fc37b25` | `7fa06ac9f631928d5232ea33397ebf90089bd55b` |
| [ci/Run-Native-Macos-Gamp4.sh](Archive/Retired/ci/Run-Native-Macos-Gamp4.sh.txt) | `1910defc7140ad98f858e256156b78abbfa980c4` | `c53dcd100cf5b55faec28364a48862479f8488a4` |
| [ci/docker/Build-Android-NDK.sh](Archive/Retired/ci/docker/Build-Android-NDK.sh.txt) | `b8b51e4dfe32baf1eb93540d59d8a4d30a54141b` | `a0777f016a0b4b4bd862a7347c8efa6a477c1cd8` |
| [ci/docker/Run-Gamp4-Container.sh](Archive/Retired/ci/docker/Run-Gamp4-Container.sh.txt) | `7a1317b01eefb89a0cd18e51d43e21eba5a76f73` | `d5f1f078838a1dee5621e980a2bd631ad6b95a88` |
| [ci/t-cross-posix.sh](Archive/Retired/ci/t-cross-posix.sh.txt) | `e2d949f719f4d3d23106e53f2db554723a518aac` | `9ae8cb94a9804c98c12a0fd03f736150c45778ed` |
