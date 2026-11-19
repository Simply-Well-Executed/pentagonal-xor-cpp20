// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Experimental photon-count gain/dispersion, exact rational statistics.
// This counts observed samples; it does not derive quantum dynamics.
#ifndef SWE_TOOLONG_PHOTON_MODEL_HPP
#define SWE_TOOLONG_PHOTON_MODEL_HPP
#include "toolong_ratio.hpp"
#include <stdexcept>
#include <vector>
namespace swe {
struct photon_count_report {
    toolong trials{0};
    toolong total_detected{0};
    toolong_ratio gain{};                // total output / total input counts
    toolong_ratio mean{};                // mean detected count per trial
    toolong_ratio dispersion{};          // population variance, count squared
    toolong_ratio fano{};                // variance / mean, if mean positive
    bool fano_defined{false};
};
[[nodiscard]] inline photon_count_report inspect_photon_counts(
    const toolong& input_per_trial, const std::vector<toolong>& observations) {
    if (input_per_trial <= toolong{0} || observations.empty())
        throw std::invalid_argument("photon model: positive input and samples required");
    photon_count_report r;
    r.trials = toolong::from_unsigned(observations.size());
    toolong squares{0};
    for (const auto& measured : observations) {
        if (measured.negative())
            throw std::invalid_argument("photon model: negative event count");
        r.total_detected += measured;
        squares += measured * measured;
    }
    r.gain = {r.total_detected, input_per_trial * r.trials};
    r.mean = {r.total_detected, r.trials};
    const toolong variance_num = r.trials * squares -
                                  r.total_detected * r.total_detected;
    if (variance_num.negative())
        throw std::logic_error("photon model: variance invariant violated");
    r.dispersion = {variance_num, r.trials * r.trials};
    r.fano_defined = !r.total_detected.is_zero();
    if (r.fano_defined) r.fano = r.dispersion / r.mean;
    return r;
}
} // namespace swe
#endif // SWE_TOOLONG_PHOTON_MODEL_HPP
