// SPDX-License-Identifier: 0BSD
// DECADI-001: direct native Go, no imports, modules, network, or package downloads.
// Separate toolchain must be officially sourced from dl.google.com/go.
package main

func shell(value byte) byte { return 0x96 ^ (0x69 ^ value) }

func main() {
    var value byte
    for {
        result := shell(value)
        if result != value^0xff || (0x69^(0x96^result)) != value {
            panic("DECADI|HOLD|xor_or_inverse")
        }
        if value == 255 {
            break
        }
        value++
    }
    original := shell(0xdb)
    mutant := shell(0xdf)
    if original == mutant || (original|0x24) != (mutant|0x24) {
        panic("DECADI|HOLD|negative_canary")
    }
}
