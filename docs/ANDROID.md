# Android release lane for the Pentagonal XOR Stability lab

This project includes the **experimental PR #17 Android Java 17 app** under
`android/` with its original 0BSD notices and unit tests. It demonstrates
the same nine-ligand / three-Pentagonal-XOR / six-byte witness as the C++20
header. The installed Android app is named `Fastest-Whisper-10D`, but that
historical experiment name does not create AI speech-to-text functionality.

## Runtime

- Local Storage Access Framework file picker, no Internet or microphone permission.
- Source must be **raw, interleaved stereo IEEE-754 float32 little-endian**.
  WAV/MP3/FLAC files must be converted **before** import. Arbitrary picker
  input is not automatically decoded.
- Nine original measurements, Stability dimension ten, three ring scores,
  six-byte hex witness, SHA-256 of input bytes and original Pearson diagnostic.
- No pretrained weights, tokenizer, ASR model, transcript or remote server.
- Invalid frame length and nonfinite samples must fail without a fake result.
- The original source is not modified.

## Building and testing

**No Gradle, no Android Gradle Plugin, no Maven, and no JUnit.**
The Android packaging process is entirely implemented in
[`ci/Build-Android.ps1`](../ci/Build-Android.ps1) using **PowerShell 7
StrictMode 2.0** and pinned SDK/JDK versions:

- Java 17 `javac` and `java` compile and execute the independent
  `StandaloneTest.java` fixture, including 2,048 independent parity
  expansions, four exact six-byte golden vectors, finite/domain checks,
  source-preservation and malformed binary-input fixtures.
- `javac -classpath android.jar` compiles the Android Activity and the
  no-weight acoustic/Stability engine.
- `aapt2 compile` + `aapt2 link` package the manifest and the
  project's one XML string resource.
- `jar` + `d8` produce `classes.dex` with min API 26; PowerShell's
  built-in .NET ZIP archive API appends the DEX without external scripts.
- `zipalign`, one-time **debug-only** `keytool` key generation,
  `apksigner sign`, and `apksigner verify` produce a verifiable APK.

The required external tools remain **JDK 17, Android SDK platform 35 and
build-tools 35.0.0**. These are identifiable development toolchain
dependencies, not a claim of *zero* external dependencies. No software
dependency can be assumed compatible with government procurement policies;
the applicable contract and supply-chain requirements require independent
review.

PowerShell 7:

```powershell
pwsh -NoLogo -NoProfile -File ./ci/Build-Android.ps1
```

The script does **not** call Gradle, its wrapper, JUnit or the AGP
plugin repository. Source and files removed during this refactor are
retained in earlier Git commits. CI also preserves failed build logs
and SHA-256 evidence, rather than silently turning errors green.

The expected unsigned-to-developer distribution binary is
`android/app/build/outputs/apk/debug/app-debug.apk`.

**Signing:** this is **debug-signed** with a CI-generated debug key, not
a long-lived release signing identity. The debug APK is suitable for
testing, **not** a production Play Store release or authenticated app
upgrade channel. Never advertise it as release-signed.

A full stable Android release would additionally require controlled
keystore custody, deterministic package metadata, device/emulator UI
tests, security/privacy review, a versioned update channel and
independently verified checksums of the inner APK. None of those is
implicitly satisfied by a green Java unit test.

## Packaging release review

CI uploads the APK as `release-android-debug`. The release job can
collect it alongside C++ platform archives and records its SHA-256 in
`SHA256SUMS`. It retains `debug` in the artifact name even on
a tag-triggered repository release, so no consumer confuses it with
a production-signed package.

**Source provenance:** the Java app and tests were copied unchanged
from [PR #17](https://github.com/pcheek1991/faster-whisper-4d-abandonedware-uniunlicensd/pull/17)
at Git SHA `432a228f1dddbb2532de1bd1f439ee8ef34af4e9`.
The C++20 mathematical rewrite is a separate new 0BSD implementation
built against the repository's existing original Pentagonal XOR header.

No synthetic test should be promoted into a speech transcription benchmark.
