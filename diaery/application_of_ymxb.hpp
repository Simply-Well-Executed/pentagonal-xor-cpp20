// SPDX-License-Identifier: 0BSD
// DEARY / DIAERY: exploratory graph transformations. RELEASE=HOLD.
// Exact source coordinates are never rewritten by display transformations.
#ifndef SWE_DIAERY_APPLICATION_OF_YMXB_HPP
#define SWE_DIAERY_APPLICATION_OF_YMXB_HPP
namespace swe::diaery::graph {
struct Point { double x; double y; };
enum class Shape { Circle, Line, VerticalLine, Invalid };
struct Fit {
    Shape shape;
    Point center;
    double radius_squared;
    double slope;
    double intercept;
    double vertical_x;
};
struct Bounds { double xmin; double xmax; double ymin; double ymax; };
[[nodiscard]] constexpr double absval(double v) noexcept { return v < 0.0 ? -v : v; }
[[nodiscard]] constexpr bool finite_domain(double v) noexcept {
    return v == v && v >= -1000000000.0 && v <= 1000000000.0;
}
[[nodiscard]] constexpr double minting(double n) noexcept {
    // EXPLORATORY: (100*n-3)/100 == n - 0.03 mathematically.
    // Input remains unchanged. Not a bank rounding or unit conversion.
    return n - 0.03;
}
[[nodiscard]] constexpr double fold(double n) noexcept { return n / 2.0; }
[[nodiscard]] constexpr double project(double n) noexcept { return (n / 2.0) * (n / 2.0); }
[[nodiscard]] constexpr double quarter(double n) noexcept { return (n * n) * (n * n) / 2.0; }
// The requested APPLICATION OF Y=MX+B: line through documented intercepts
// (0,Y) and (X,0). It is an algebraic construction, NOT a measured
// physiological correlation. X and Y must be positive inches.
struct InterceptLine { bool valid; double slope; double intercept; Point x_end; Point y_end; };
[[nodiscard]] constexpr InterceptLine application_of_ymxb(double x,double y) noexcept {
    if(!finite_domain(x)||!finite_domain(y)||x<=0.0||y<=0.0)
        return {false,0.,0.,{0.,0.},{0.,0.}};
    const double m=-y/x;
    if(!finite_domain(m))return {false,0.,0.,{0.,0.},{0.,0.}};
    return {true,m,y,{x,0.},{0.,y}};
}
[[nodiscard]] constexpr Fit fit(Point a, Point b, Point c) noexcept {
    Fit out{Shape::Invalid,{0.0,0.0},0.0,0.0,0.0,0.0};
    if (!finite_domain(a.x)||!finite_domain(a.y)||
        !finite_domain(b.x)||!finite_domain(b.y)||
        !finite_domain(c.x)||!finite_domain(c.y)) return out;
    const double d=2.0*(a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y));
    const double dx1=b.x-a.x,dy1=b.y-a.y,dx2=c.x-a.x,dy2=c.y-a.y;
    const double scale=absval(dx1)+absval(dy1)+absval(dx2)+absval(dy2);
    if (scale==0.0) return out; // coincident: infinitely many lines
    // Error tolerance applies only to the classifier, never source values.
    if (absval(d)>1.0e-12*scale*scale) {
        const double aa=a.x*a.x+a.y*a.y;
        const double bb=b.x*b.x+b.y*b.y;
        const double cc=c.x*c.x+c.y*c.y;
        const double ux=(aa*(b.y-c.y)+bb*(c.y-a.y)+cc*(a.y-b.y))/d;
        const double uy=(aa*(c.x-b.x)+bb*(a.x-c.x)+cc*(b.x-a.x))/d;
        const double r2=(a.x-ux)*(a.x-ux)+(a.y-uy)*(a.y-uy);
        if (!finite_domain(ux)||!finite_domain(uy)||!finite_domain(r2)) return out;
        out.shape=Shape::Circle;out.center={ux,uy};out.radius_squared=r2;
        return out;
    }
    // Collinear fallback; choose a distinct pair to avoid zero division.
    Point p=a,q=b;
    if (p.x==q.x && p.y==q.y) q=c;
    if (p.x==q.x && p.y==q.y) return out;
    if (p.x==q.x) {out.shape=Shape::VerticalLine;out.vertical_x=p.x;return out;}
    out.slope=(q.y-p.y)/(q.x-p.x);
    out.intercept=p.y-out.slope*p.x;
    if (!finite_domain(out.slope)||!finite_domain(out.intercept)) return out;
    out.shape=Shape::Line;
    return out;
}
[[nodiscard]] constexpr Bounds include(Bounds b,Point p) noexcept {
    if(p.x<b.xmin)b.xmin=p.x;
    if(p.x>b.xmax)b.xmax=p.x;
    if(p.y<b.ymin)b.ymin=p.y;
    if(p.y>b.ymax)b.ymax=p.y;
    return b;
}
[[nodiscard]] constexpr Bounds view_bounds(Point a,Point b,Point c) noexcept {
    // Derived display only: no mutation of coordinate records.
    Bounds box{a.x,a.x,a.y,a.y};
    box=include(box,b);box=include(box,c);
    double width=box.xmax-box.xmin,height=box.ymax-box.ymin;
    const double side=(width>height?width:height)+1.0;
    const double mx=(box.xmin+box.xmax)/2.0,my=(box.ymin+box.ymax)/2.0;
    return {mx-side/2.0,mx+side/2.0,my-side/2.0,my+side/2.0};
}
} // namespace swe::diaery::graph
#endif
