// SPDX-License-Identifier: 0BSD
#include "../shared/sigma_form_geometry.hpp"
using namespace swe::sigma_form;
constexpr bool witness() {
    constexpr double radii[]={0.25,1,2,4,16};
    constexpr double alphas[]={0,0.125,1};
    constexpr double powers[]={0,1,10,1000};
    for(double r:radii) for(double a:alphas) {
        const auto p=prepare_inverse_square(a,r,geometry_pi);
        if(!p.valid) return false;
        for(double w:powers) {
            const double reference=w*attenuation(a*r).value/(4.0*geometry_pi*r*r);
            const auto result=p.evaluate(w);
            const auto single=inverse_square_intensity(w,a,r,geometry_pi);
            if(!result.valid || !single.valid || result.value!=reference ||
               single.value!=reference) return false;
        }
    }
    const auto p=prepare_inverse_square(0,1,geometry_pi);
    const auto q=prepare_inverse_square(0,2,geometry_pi);
    return p.evaluate(4).value==q.evaluate(16).value &&
           !p.evaluate(-1).valid &&
           !prepare_inverse_square(0,0,geometry_pi).valid &&
           !prepare_inverse_square(-1,1,geometry_pi).valid &&
           !prepare_inverse_square(1,17,geometry_pi).valid;
}
static_assert(witness());
int main() { return witness()?0:1; }
