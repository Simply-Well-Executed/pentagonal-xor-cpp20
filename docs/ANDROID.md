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

Requirements: JDK 17, Android SDK platform 35, build-tools 35.0.0, Gradle
8.11.1, and a suitable Android Gradle Plugin as declared under `android/`.
GitHub Actions builds the debug APK and executes JUnit + Android lint with
those toolchain versions.

PowerShell 7:

```powershell
gradle -p android --no-daemon :app:testDebugUnitTest :app:lintDebug :app:assembleDebug
```

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
