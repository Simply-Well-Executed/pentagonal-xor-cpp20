/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Experimental VISTA 9x9: nine six-gate loops and three read-only reports.
// Does NOT alter the canonical parity scorer or the six-stage Machine.
// Every native loop ends with FLOP. The separate MINZA terminal-route
// FLOP/FLIP metadata belongs to sigma_penta_ping.hpp, not this Machine.
#ifndef SWE_PENTAGONAL_VISTA_9X9_HPP
#define SWE_PENTAGONAL_VISTA_9X9_HPP

#include "pentagonal_gates.hpp"
#include <array>
#include <cstdint>

namespace swe::pentagonal::vista9x9 {

inline constexpr unsigned loop_count = 9u;
inline constexpr unsigned gate_count = 6u;
inline constexpr unsigned vista_pass_count = 3u;
inline constexpr unsigned extended_steps_per_loop = gate_count + vista_pass_count;

enum class Pass : std::uint8_t { capture = 0, verify = 1, synthesize = 2 };

struct Input {
    std::uint32_t observed{};      // original 30-bit COR data (six blocks of five)
    std::uint32_t expected{};      // separately supplied comparison, NOT an authenticated witness
    std::uint64_t source_token{};  // caller-defined provenance handle, NOT a digest
    Entry entry{Entry::or_gate};
    bool witness_present{false};  // reports claim only; not cryptographic verification
    bool sequence_fresh{true};
    bool clock_nondecreasing{true};
};

struct GateView {
    Stage stage{};
    bool output{};
    std::uint64_t completed_loops{};
};

struct VistaPass {
    Pass pass{};
    std::array<GateView, gate_count> history{};
    Vista cor{};                   // read-only copy of original COR observation
    std::uint64_t source_token{};
    std::uint32_t observed{}, expected{};
    bool structurally_consistent{};
    bool capture_complete{};       // phase A
    bool verification_complete{};  // phase B and carried to C
    bool synthesis_complete{};     // phase C
    std::uint8_t active_gate_outputs{}; // phase C only
    std::uint16_t anomaly_flags{};       // phase C only; never repaired
    bool witness_reported{};
    bool independently_authenticated{}; // always false without external proof
};

struct Loop {
    std::array<Step, gate_count> gates{};
    std::array<VistaPass, vista_pass_count> views{};
};

struct Report {
    std::array<Loop, loop_count> loops{};
    std::uint64_t gate_ticks{};
    std::uint64_t vista_ticks{};
    std::uint64_t extended_ticks{};
    std::uint64_t gate_observations{};
    std::uint64_t complete_loops{};
    bool control_flow_consistent{true};
};

inline constexpr std::array<Stage, gate_count> stages{
    Stage::xor_gate, Stage::or_gate, Stage::and_gate,
    Stage::nop, Stage::cor, Stage::flop
};

inline bool same_vista(const Vista& a, const Vista& b) noexcept {
    if (a.operand != b.operand || a.observed != b.observed ||
        a.expected != b.expected || a.rupture != b.rupture ||
        a.evaluations != b.evaluations || a.entry != b.entry ||
        a.witness_present != b.witness_present ||
        a.valid_entry != b.valid_entry) return false;
    for (unsigned i=0; i<6u; ++i) {
        if (a.blocks[i].xor_value != b.blocks[i].xor_value ||
            a.blocks[i].or_value != b.blocks[i].or_value ||
            a.blocks[i].and_value != b.blocks[i].and_value) return false;
    }
    return true;
}

// This is an observational adapter. It schedules 6 native ticks, then
// 3 VISTA reporting ticks. No additional XOR executes at FLOP.
inline Report execute(const std::array<Input, loop_count>& source) noexcept {
    Report report{};
    Machine machine{};

    for (unsigned cycle=0; cycle<loop_count; ++cycle) {
        const Input& original = source[cycle];
        Loop& row = report.loops[cycle];
        bool sequence_ok = true;
        for (unsigned gate=0; gate<gate_count; ++gate) {
            Step step = machine.tick(original.observed, original.expected,
                                     original.entry, original.witness_present,
                                     original.sequence_fresh,
                                     original.clock_nondecreasing);
            sequence_ok = sequence_ok && (step.stage == stages[gate]);
            row.gates[gate] = step;
            ++report.gate_ticks;
        }
        sequence_ok = sequence_ok &&
                      machine.next == Stage::xor_gate &&
                      machine.loops == static_cast<std::uint64_t>(cycle+1u);
        const Vista& cor = row.gates[4].vista;
        const Gates g = boolean_gates(
            static_cast<std::uint8_t>(original.observed & 31u));
        const bool expected_output[gate_count] = {
            g.xor_value, g.or_value, g.and_value,
            g.and_value, g.and_value, g.and_value
        };
        bool gate_values_ok = true;
        for (unsigned gate=0; gate<gate_count; ++gate) {
            gate_values_ok = gate_values_ok &&
                             row.gates[gate].output == expected_output[gate];
        }
        const bool cor_ok = cor.observed == (original.observed & ligand_mask) &&
                            cor.expected == (original.expected & ligand_mask) &&
                            cor.entry == original.entry &&
                            cor.witness_present == original.witness_present &&
                            entry_of(cor.operand) == original.entry &&
                            ligands_of(cor.operand) == cor.observed &&
                            ruptures_of(cor.operand) == cor.rupture &&
                            evaluations_of(cor.operand) == cor.evaluations &&
                            same_vista(cor, pentagonal_vista(cor));

        const bool consistent = sequence_ok && gate_values_ok && cor_ok;
        report.control_flow_consistent =
            report.control_flow_consistent && consistent;
        // Every VISTA pass reads ALL six gate records and the COR witness.
        for (unsigned p=0; p<vista_pass_count; ++p) {
            VistaPass& v = row.views[p];
            v.pass = static_cast<Pass>(p);
            for (unsigned gate=0; gate<gate_count; ++gate) {
                v.history[gate] = {row.gates[gate].stage,
                                   row.gates[gate].output,
                                   row.gates[gate].loops_completed};
                ++report.gate_observations;
            }
            v.cor = pentagonal_vista(cor);
            v.source_token = original.source_token;
            v.observed = original.observed; // preserve caller's full input
            v.expected = original.expected;
            v.structurally_consistent = consistent;
            v.capture_complete = true;
            v.verification_complete = (p >= 1u) && consistent;
            v.synthesis_complete = (p == 2u) && consistent;
            if (p == 2u) {
                for (unsigned gate=0; gate<gate_count; ++gate)
                    v.active_gate_outputs = static_cast<std::uint8_t>(
                        v.active_gate_outputs + (v.history[gate].output ? 1u : 0u));
                v.anomaly_flags = cor.rupture;
            }
            v.witness_reported = original.witness_present;
            // Mere witness_present booleans, local hashes or matching
            // expected values never establish independently verified origin.
            v.independently_authenticated = false;
            ++report.vista_ticks;
        }
    }
    report.complete_loops = machine.loops;
    report.extended_ticks = report.gate_ticks + report.vista_ticks;
    return report;
}

static_assert(loop_count * extended_steps_per_loop == 81u);
static_assert(loop_count * gate_count == 54u);
static_assert(loop_count * vista_pass_count == 27u);

} // namespace swe::pentagonal::vista9x9
#endif
