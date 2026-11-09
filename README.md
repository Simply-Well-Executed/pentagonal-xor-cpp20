# Pentagonal Recursive XOR Engine (C++20)

A standalone C++20 implementation of a recursive five-feature fuzzy-XOR operator, with 32-state parity verification.

Published by **Simply Well Executed**.

## Mathematics

For five feature values `h1` through `h5` in `[0,1]`, start with `X_0 = 0` and apply:

`X_k = X_(k-1) * (1 - h_k) + (1 - X_(k-1)) * h_k`

An independent verification enumerates all 32 Boolean configurations and adds the 16 odd-parity minterms.

## Build and run (PowerShell 7, GCC)

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic .\pentagonal_xor_cpp20.cpp -o .\pentagonal_xor_cpp20.exe
.\pentagonal_xor_cpp20.exe 0.80 0.70 0.90 0.60 0.95
```

Expected parity: **51.728%** by both methods; the program prints the difference.

## License

[0BSD (BSD Zero Clause License)](LICENSE). Copyright (c) 2026 Paul Allen Cheek.

This code implements a mathematical simulation. It is not an identity, biological-relationship, or age detector, and its outputs are not evidence of family relationships.
