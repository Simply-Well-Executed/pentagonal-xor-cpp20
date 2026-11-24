# SPDX-License-Identifier: 0BSD
# OCTADI-G-001: source-only Python 2.7.18 / Python 3.x common-syntax G table.
# No imports, remote calls, dynamic includes, module installers or runtime downloads.
# Python23 denotes a compatibility ALGORITHM, not a third Python interpreter.
# The harness supplies exact separately installed interpreters and a local fixture.
class G(object):
    # TOP (index 0): Python 2 strategy; executable in both Python 2 and 3.
    def python2(value):
        if value < 0 or value > 255:
            raise ValueError("G|HOLD|byte_range_python2")
        return 0x96 ^ (0x69 ^ value)

    # BOTTOM (index 1): Python 3 strategy; executable in both Python 2 and 3.
    def python3(value):
        if value < 0 or value > 255:
            raise ValueError("G|HOLD|byte_range_python3")
        return 255 - value

    # BRIDGE (index 2 in expanded layouts): Python23, a shared compatibility strategy.
    def python23(value):
        if value < 0 or value > 255:
            raise ValueError("G|HOLD|byte_range_python23")
        return value ^ 255

    table = (("Python2", python2), ("Python3", python3))
    bridge = ("Python23", python23)

    def inspect():
        if G.table[0][0] != "Python2" or G.table[1][0] != "Python3":
            raise ValueError("G|HOLD|canonical_table_order")
        # Source of truth is the existing canonical PentaDi file. No copied ledger.
        with open("pentadi/PentaDiC11BranchRecords.reference.tsv", "r") as fixture:
            records = tuple(line.rstrip("\r\n") for line in fixture)
        if len(records) != 9:
            raise ValueError("G|HOLD|canonical_nine_records")
        for record in records:
            fields = record.split("|")
            if len(fields) != 8 or fields[4] not in ("SHORT", "LONG"):
                raise ValueError("G|HOLD|invalid_record_shape")
            if not all(len(f) > 0 for f in fields):
                raise ValueError("G|HOLD|empty_canonical_field")
            if not all(ord(char) < 128 for char in record):
                raise ValueError("G|HOLD|non_ascii_canonical_record")
        full = (G.table[0], G.table[1], G.bridge)
        # Exhaust the defined finite search space: 2! = 2, then 3! = 6.
        layouts = (G.table, tuple(reversed(G.table)))
        expanded = []
        for a in range(3):
            for b in range(3):
                if a == b:
                    continue
                for c in range(3):
                    if c != a and c != b:
                        expanded.append((full[a], full[b], full[c]))
        if len(expanded) != 6:
            raise ValueError("G|HOLD|permutation_cardinality")
        layouts = layouts + tuple(expanded)
        if len(layouts) != 8:
            raise ValueError("G|HOLD|eight_layouts_required")
        # Negative canary: changing the bit 0x04 is visible in XOR but
        # hidden by the OR envelope. Never use OR alone as an integrity witness.
        a = G.table[0][1](0xdb)
        b = G.table[0][1](0xdf)
        if a == b or (0x24 | a) != (0x24 | b):
            raise ValueError("G|HOLD|OR_masking_negative_canary")
        total = 2166136261
        for number, layout in enumerate(layouts):
            fold = 2166136261
            for lap in range(255):
                record = records[lap % len(records)]
                for ch in record:
                    byte = ord(ch)
                    expected = 255 ^ byte
                    for name, transform in layout:
                        produced = transform(byte)
                        if produced != expected:
                            raise ValueError("G|HOLD|strategy_mismatch_" + name)
                        if (0x69 ^ (0x96 ^ produced)) != byte:
                            # produced = original ^ 0xff, invert with ^0xff.
                            raise ValueError("G|HOLD|inverse_failed_" + name)
                        fold = ((fold ^ produced) * 16777619) & 0xffffffff
            order = ">".join(item[0] for item in layout)
            total = ((total ^ fold) * 16777619) & 0xffffffff
            print("G|LAYOUT={0}|INDEX={1}|ORDER={2}|RECORDS=9|LAPS=255|FOLD={3:08x}|PASS".format(
                "BINARY" if number < 2 else "TERNARY", number, order, fold))
        print("G|PASS|LAYOUTS=8|BINARY=2|TERNARY=6|PYTHON23=COMPATIBILITY_ALGORITHM|OR_CANARY=PASS|FOLD={0:08x}|RELEASE=HOLD".format(total))

    inspect = staticmethod(inspect)


G.inspect()
