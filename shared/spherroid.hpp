// SPDX-License-Identifier: 0BSD
#pragma once
namespace MSL {
struct SPHERROID final {
    struct Point { double x,y,z; };
    struct Result { Point source, projected; double residual; bool valid; };
    static constexpr double ABS(double x) noexcept { return x<0?-x:x; }
    static constexpr double MAX(double a,double b) noexcept { return a>b?a:b; }
    // Bounded square root for normalized squared radii in [1,3].
    static constexpr double ROOT(double x) noexcept {
        double r=2;
        for(unsigned i=0;i<8;++i) r=(r+x/r)*0.5;
        return r;
    }
    // Unit cube boundary, centered at VERIFY. Source never overwritten.
    // Radial deviation from unit sphere decays by HexaDi.
    static constexpr Result PROJECT(Point p,double k,double t) noexcept {
        Result out{p,p,0,false};
        if (!(ABS(p.x)<=1 && ABS(p.y)<=1 && ABS(p.z)<=1 &&
              k>=0 && k<=1e6 && t>=0 && t<=1e6)) return out;
        if(MAX(MAX(ABS(p.x),ABS(p.y)),ABS(p.z))!=1) return out;
        const double radius=ROOT(p.x*p.x+p.y*p.y+p.z*p.z);
        const double residual=1/(1+k*t);
        const double scale=residual+(1-residual)/radius;
        out.projected={p.x*scale,p.y*scale,p.z*scale};
        out.residual=residual;
        out.valid=true;
        return out;
    }
    // RECTANGLOID 60 x 15 x 15: normalize source coordinates first.
    static constexpr Result RECTANGLOID(double x,double y,double z,
                                        double k,double t) noexcept {
        return PROJECT({(x-30)/30,(y-7.5)/7.5,(z-7.5)/7.5},k,t);
    }
};
}
