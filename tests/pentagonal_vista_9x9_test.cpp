// SPDX-License-Identifier: 0BSD
#include "../shared/pentagonal_vista_9x9.hpp"
#include <array>
#include <cstdint>
#include <iostream>

using namespace swe::pentagonal;
using namespace swe::pentagonal::vista9x9;

int main() {
    for (unsigned mask=0; mask<32u; ++mask) {
        std::array<Input,loop_count> source{};
        for (unsigned n=0; n<loop_count; ++n) {
            source[n].observed = mask | (mask << 5u) | (mask << 10u);
            source[n].expected = source[n].observed;
            source[n].source_token = 0x900000u + n;
            source[n].witness_present = false;
        }
        const auto original = source;
        const Report result = execute(source);
        if (result.gate_ticks != 54u || result.vista_ticks != 27u ||
            result.extended_ticks != 81u ||
            result.gate_observations != 162u ||
            result.complete_loops != 9u || !result.control_flow_consistent)
            return 1;
        for (unsigned n=0; n<loop_count; ++n) {
            if (source[n].observed != original[n].observed ||
                source[n].expected != original[n].expected ||
                source[n].source_token != original[n].source_token)
                return 2;
            const Gates truth = boolean_gates(static_cast<std::uint8_t>(mask));
            const bool outputs[] = {
                truth.xor_value, truth.or_value, truth.and_value,
                truth.and_value, truth.and_value, truth.and_value
            };
            for (unsigned gate=0; gate<gate_count; ++gate) {
                const auto& s = result.loops[n].gates[gate];
                if (s.stage != stages[gate] || s.output != outputs[gate])
                    return 3;
                if (s.loops_completed != (n+((gate==5u)?1u:0u)))
                    return 4;
            }
            const Vista& canonical = result.loops[n].gates[4].vista;
            for (unsigned p=0; p<vista_pass_count; ++p) {
                const VistaPass& v = result.loops[n].views[p];
                if (v.pass != static_cast<Pass>(p) ||
                    v.source_token != original[n].source_token ||
                    v.observed != original[n].observed ||
                    v.expected != original[n].expected ||
                    !v.structurally_consistent || !v.capture_complete ||
                    v.verification_complete != (p>=1u) ||
                    v.synthesis_complete != (p==2u) ||
                    v.witness_reported || v.independently_authenticated ||
                    !same_vista(v.cor,canonical) ||
                    !(v.cor.rupture & missing_witness))
                    return 5;
                unsigned true_outputs=0;
                for (bool bit: outputs) true_outputs+=bit?1u:0u;
                if (p==2u && (v.active_gate_outputs!=true_outputs ||
                              v.anomaly_flags!=canonical.rupture)) return 9;
                if (p!=2u && (v.active_gate_outputs!=0u ||
                               v.anomaly_flags!=0u)) return 10;
                for (unsigned gate=0; gate<gate_count; ++gate)
                    if (v.history[gate].stage != stages[gate] ||
                        v.history[gate].output != outputs[gate])
                        return 6;
            }
        }
    }
    std::array<Input,loop_count> adversarial{};
    for (unsigned i=0; i<loop_count; ++i) {
        adversarial[i].observed = ligand_mask;
        adversarial[i].expected = ligand_mask;
        adversarial[i].source_token = 100+i;
        adversarial[i].witness_present = true;
    }
    // A mismatch must remain visible in the COR/VISTA outputs.
    adversarial[2].observed &= ~(1u << 2u);
    adversarial[5].sequence_fresh = false;
    adversarial[6].clock_nondecreasing = false;
    adversarial[7].entry = Entry::fault;
    const Report r = execute(adversarial);
    if (!r.control_flow_consistent || r.extended_ticks != 81u ||
        r.loops[2].views[2].cor.rupture != 1u ||
        !(r.loops[5].views[1].cor.rupture & replay_sequence) ||
        !(r.loops[6].views[1].cor.rupture & clock_regression) ||
        !(r.loops[7].views[1].cor.rupture & invalid_entry))
        return 7;
    // No local witness report alone proves independent authenticity.
    for (const auto& loop : r.loops)
        for (const auto& pass : loop.views)
            if (pass.independently_authenticated) return 8;
    std::cout << "PASS|VISTA-9X9|loops=9|gates=54|views=27|stages=81|"
              << "gate_observations=162|states=32|source_unchanged=1|"
              << "independent_authentication_claim=0\n";
    return 0;
}
