# Contributing

Thank you for improving the Pentagonal Recursive XOR Engine!

1. Fork the repository and open a topic branch.
2. Keep the implementation dependency-free and compatible with C++20.
3. Build with CMake, run \`ctest --test-dir build --output-on-failure\`, and include tests for behavioral changes.
4. Open a pull request describing the change and any platform impact.

GitHub Actions runs the Linux, Windows, and macOS build matrix on pull requests. A successful main-branch build creates a preview prerelease, while version tags (\`v*\`) publish versioned releases.

The project is licensed under [0BSD](LICENSE).
