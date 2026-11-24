(* SPDX-License-Identifier: 0BSD *)
(* TRIDECADI-001: OCaml native ocamlopt -O2; no opam, external libraries or imports. *)
let shell value = 0x96 lxor (0x69 lxor value)
let () =
  for value = 0 to 255 do
    let transformed = shell value in
    if transformed <> (value lxor 255) then failwith "TRIDECADI|HOLD|xor";
    if (0x69 lxor (0x96 lxor transformed)) <> value then
      failwith "TRIDECADI|HOLD|inverse"
  done;
  let a = shell 0xdb in
  let b = shell 0xdf in
  if a = b || ((a lor 0x24) <> (b lor 0x24)) then
    failwith "TRIDECADI|HOLD|negative_canary";
  print_endline "TRIDECADI|PASS|NATIVE_OCAML|BYTES=256|RELEASE=HOLD"
