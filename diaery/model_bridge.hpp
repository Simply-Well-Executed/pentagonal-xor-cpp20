// SPDX-License-Identifier: 0BSD
// Sole geometry authority: reuse existing project source, never recopy formulas.
// No C++ standard library or system headers.
#ifndef SWE_DIAERY_MODEL_BRIDGE_HPP
#define SWE_DIAERY_MODEL_BRIDGE_HPP
#include "../shared/sigma_form_geometry.hpp"
#include "../shared/sigma_form_area_q62.hpp"
#include "../shared/right_triangle_f123_original.hpp"
#include "../shared/f123_eighth_fold_experiment.hpp"
#include "../shared/sigma_e_redaxualchoice.hpp"
#include "../shared/sigma_f_redaxualchoice.hpp"
#include "../shared/four_pyramid_reduxualchoice.hpp"
#include "../shared/u_circle_fit.hpp"
#include "../shared/xdr_rdx_fold_tork.hpp"
#include "../shared/minza_direct_ping_bidi.hpp"
#include "../shared/minza_direct_ping_tridi.hpp"
#include "../shared/three_geometric_folds.hpp"
#include "../shared/rectangloid_primitive_cxx20.hpp"
#include "../shared/toolong_q62_core.hpp"
#include "../shared/slidingtoolong.hpp"
#include "strict_profile.hpp"
#include "harmony.hpp"
namespace swe::diaery::geometry {
namespace form=swe::three_geometric_folds;
namespace solid=swe::rectangloid_primitive;
namespace nice=swe::u_circle_fit;
namespace bidi=swe::minza_direct_ping_bidi;
namespace tridi=swe::minza_direct_ping_tridi;
namespace tork=swe::xdr_rdx_fold_tork;
namespace slide=swe::slidingtoolong;
namespace harm=swe::diaery::harmony;
static_assert(slide::QUALIFIED());
static_assert(harm::INTEGRITY());
static_assert(form::WITNESS.valid);
static_assert(solid::WITNESS.valid);
static_assert(nice::TRIANGLE_RESULT==nice::U::Nice);
static_assert(nice::VALIDATE(nice::FACE,nice::HEIGHT_CIRCLE)==nice::U::NotNice);
static_assert(bidi::REPORT.ping_after_restore);
static_assert(tridi::REPORT.valid);
static_assert(tork::SIGMA_SIGMA_SIGMA(64u));
static_assert(solid::VOLUME==13500);
static_assert(solid::SURFACE_AREA==4050);
static_assert(solid::ALL_EDGES_PAIRED(solid::CLOSED));
static_assert(solid::MISSING_CAP_REJECTED());
static_assert(!solid::RELEASE_AUTHORIZED);
}
#endif
