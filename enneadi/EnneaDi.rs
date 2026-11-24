// SPDX-License-Identifier: 0BSD
// ENNEADI-001: native Rust compiler only, no Cargo, registry or downloads.
#![forbid(unsafe_code)]
fn shell(value: u8) -> u8 { 0x96u8 ^ (0x69u8 ^ value) }
fn main() {
    let mut value: u8 = 0;
    loop {
        let result = shell(value);
        assert_eq!(result, value ^ 0xffu8, "ENNEADI|HOLD|independent_xor");
        assert_eq!(0x69u8 ^ (0x96u8 ^ result), value, "ENNEADI|HOLD|inverse");
        if value == 255 { break; }
        value += 1;
    }
    let original = shell(0xdb);
    let mutated = shell(0xdf);
    assert_ne!(original, mutated, "ENNEADI|HOLD|xor_canary");
    assert_eq!(original | 0x24, mutated | 0x24, "ENNEADI|HOLD|or_collision");
    println!("ENNEADI|PASS|NATIVE_RUST|BYTES=256|NO_CARGO|RELEASE=HOLD");
}
