// SPDX-License-Identifier: 0BSD
// SIGMA-FORM-EXPLORATORY-PR34-001: include-free C++20 mathematical forms.
// This is NOT an image reconstruction or a measurement of the photographs.
// Q62 records have one canonical source: toolong_q62_core.hpp.
#ifndef SWE_SIGMA_FORM_GEOMETRY_HPP
#define SWE_SIGMA_FORM_GEOMETRY_HPP
namespace swe::sigma_form {
inline constexpr double geometry_pi = 3.14159265358979323846264338327950288;
inline constexpr double largest_double = 1.7976931348623157e308;
[[nodiscard]] constexpr double absolute(double x) noexcept {
    return x < 0.0 ? -x : x;
}
[[nodiscard]] constexpr double maximum(double x, double y) noexcept {
    return x > y ? x : y;
}
[[nodiscard]] constexpr bool finite(double x) noexcept {
    return x == x && x >= -largest_double && x <= largest_double;
}
[[nodiscard]] constexpr bool near(double a, double b, double tolerance) noexcept {
    return finite(a) && finite(b) && finite(tolerance) &&
           tolerance >= 0.0 && absolute(a - b) <= tolerance;
}
struct Scalar { double value; bool valid; };
struct Vec2 { double x; double y; };
struct Vec3 { double x; double y; double z; };
struct Point2 { Vec2 value; bool valid; };
struct Arm2 { Vec2 elbow; Vec2 hand; bool valid; };
struct Pinhole { double fx; double fy; double cx; double cy; };

// Bounded sine Taylor series, with radians within [-64,64].
[[nodiscard]] constexpr Scalar sine(double radians) noexcept {
    if (!finite(radians) || absolute(radians) > 64.0)
        return {0.0, false};
    double x = radians;
    while (x > geometry_pi) x -= 2.0 * geometry_pi;
    while (x < -geometry_pi) x += 2.0 * geometry_pi;
    double term = x;
    double sum = x;
    for (unsigned n = 1; n <= 14; ++n) {
        const double d = 2.0 * static_cast<double>(n);
        term *= -(x*x) / (d * (d+1.0));
        sum += term;
    }
    return {sum, finite(sum)};
}
[[nodiscard]] constexpr Scalar cosine(double radians) noexcept {
    return sine(radians + geometry_pi * 0.5);
}
// e^(-x) via reciprocal of a positive series; 0 <= x <= 16.
[[nodiscard]] constexpr Scalar attenuation(double x) noexcept {
    if (!finite(x) || x < 0.0 || x > 16.0)
        return {0.0, false};
    double term = 1.0, sum = 1.0;
    for (unsigned n = 1; n <= 64; ++n) {
        term *= x / static_cast<double>(n);
        sum += term;
    }
    if (!finite(sum) || sum <= 0.0) return {0.0, false};
    return {1.0 / sum, true};
}
// z = height*(1 - max(abs(x-cx),abs(y-cy))/half_base).
// Coordinates must lie in a square base, height >= 0, half_base > 0.
[[nodiscard]] constexpr Scalar square_pyramid_z(
    double x, double y, double cx, double cy,
    double half_base, double height) noexcept {
    if (!finite(x) || !finite(y) || !finite(cx) || !finite(cy) ||
        !finite(half_base) || !finite(height) ||
        half_base <= 0.0 || height < 0.0) return {0.0, false};
    const double radius = maximum(absolute(x-cx), absolute(y-cy));
    if (!finite(radius) || radius > half_base) return {0.0, false};
    const double z = height * (1.0-radius/half_base);
    return {z, finite(z)};
}
// B(t)=(1-t)^2*p0 + 2*t*(1-t)*p1 + t^2*p2.
[[nodiscard]] constexpr Point2 quadratic_bezier(
    Vec2 p0, Vec2 p1, Vec2 p2, double t) noexcept {
    if (!finite(p0.x) || !finite(p0.y) ||
        !finite(p1.x) || !finite(p1.y) ||
        !finite(p2.x) || !finite(p2.y) ||
        !finite(t) || t < 0.0 || t > 1.0)
        return {{0.0,0.0},false};
    const double u = 1.0-t;
    const Vec2 point{
        u*u*p0.x + 2.0*u*t*p1.x + t*t*p2.x,
        u*u*p0.y + 2.0*u*t*p1.y + t*t*p2.y
    };
    return {point,finite(point.x) && finite(point.y)};
}
// One pleat: x_k(y)=center + amplitude*sin(frequency*y+phase).
[[nodiscard]] constexpr Scalar pleat_x(
    double y, double center, double amplitude,
    double frequency, double phase) noexcept {
    if (!finite(y) || !finite(center) || !finite(amplitude) ||
        !finite(frequency) || !finite(phase)) return {0.0,false};
    const Scalar oscillation = sine(frequency*y+phase);
    if (!oscillation.valid) return {0.0,false};
    const double value = center + amplitude*oscillation.value;
    return {value,finite(value)};
}
// d(s,t)=amplitude*exp(-kappa*s)*sin(k*s-omega*t+phase).
// This is a parameterized deformation, not cloth physics.
[[nodiscard]] constexpr Scalar damped_wave(
    double s, double t, double amplitude, double kappa,
    double spatial_frequency, double angular_speed,
    double phase) noexcept {
    if (!finite(s) || !finite(t) || !finite(amplitude) ||
        !finite(kappa) || !finite(spatial_frequency) ||
        !finite(angular_speed) || !finite(phase) ||
        s < 0.0 || kappa < 0.0) return {0.0,false};
    const Scalar decay = attenuation(kappa*s);
    const Scalar wave = sine(spatial_frequency*s - angular_speed*t + phase);
    if (!decay.valid || !wave.valid) return {0.0,false};
    const double value = amplitude*decay.value*wave.value;
    return {value,finite(value)};
}
// Two-link planar arm; elbow angle is relative to the upper segment.
[[nodiscard]] constexpr Arm2 two_link_arm(
    Vec2 shoulder, double upper_length, double lower_length,
    double shoulder_angle, double elbow_relative_angle) noexcept {
    if (!finite(shoulder.x) || !finite(shoulder.y) ||
        !finite(upper_length) || !finite(lower_length) ||
        upper_length < 0.0 || lower_length < 0.0)
        return {{0.0,0.0},{0.0,0.0},false};
    const Scalar c1 = cosine(shoulder_angle);
    const Scalar s1 = sine(shoulder_angle);
    const Scalar c2 = cosine(shoulder_angle+elbow_relative_angle);
    const Scalar s2 = sine(shoulder_angle+elbow_relative_angle);
    if (!c1.valid || !s1.valid || !c2.valid || !s2.valid)
        return {{0.0,0.0},{0.0,0.0},false};
    const Vec2 elbow{shoulder.x+upper_length*c1.value,
                     shoulder.y+upper_length*s1.value};
    const Vec2 hand{elbow.x+lower_length*c2.value,
                    elbow.y+lower_length*s2.value};
    return {elbow,hand,finite(elbow.x) && finite(elbow.y) &&
                       finite(hand.x) && finite(hand.y)};
}
// Ia + reflectance*E*max(0,dot(normal,light)).
// Both supplied vectors must be unit length.
[[nodiscard]] constexpr Scalar lambert(
    double ambient, double reflectance, double irradiance,
    Vec3 normal, Vec3 light) noexcept {
    if (!finite(ambient) || !finite(reflectance) || !finite(irradiance) ||
        ambient < 0.0 || reflectance < 0.0 || reflectance > 1.0 ||
        irradiance < 0.0 || !finite(normal.x) || !finite(normal.y) ||
        !finite(normal.z) || !finite(light.x) || !finite(light.y) ||
        !finite(light.z)) return {0.0,false};
    const double nn = normal.x*normal.x + normal.y*normal.y + normal.z*normal.z;
    const double ll = light.x*light.x + light.y*light.y + light.z*light.z;
    if (!near(nn,1.0,1e-9) || !near(ll,1.0,1e-9))
        return {0.0,false};
    const double dot = normal.x*light.x + normal.y*light.y +
                       normal.z*light.z;
    const double value = ambient + reflectance*irradiance*maximum(0.0,dot);
    return {value,finite(value)};
}
// Simple perspective projection; requires calibrated camera inputs.
[[nodiscard]] constexpr Point2 pinhole_project(
    Vec3 point, Pinhole camera) noexcept {
    if (!finite(point.x) || !finite(point.y) || !finite(point.z) ||
        !finite(camera.fx) || !finite(camera.fy) ||
        !finite(camera.cx) || !finite(camera.cy) ||
        point.z <= 0.0 || camera.fx <= 0.0 || camera.fy <= 0.0)
        return {{0.0,0.0},false};
    const Vec2 image{camera.fx*point.x/point.z+camera.cx,
                     camera.fy*point.y/point.z+camera.cy};
    return {image,finite(image.x) && finite(image.y)};
}
// Real sample: P*exp(-alpha*omega)/(4*pi*omega^2).
// P in W and omega in metres give W/m^2 if alpha has inverse-length units.
// It is separate from the exact Q62 fixed-point Intensity record.
[[nodiscard]] constexpr Scalar inverse_square_intensity(
    double power, double alpha, double omega, double pi_value) noexcept {
    if (!finite(power) || !finite(alpha) || !finite(omega) ||
        !finite(pi_value) || power < 0.0 || alpha < 0.0 ||
        omega <= 0.0 || pi_value <= 0.0) return {0.0,false};
    const Scalar decay = attenuation(alpha*omega);
    const double denom = 4.0*pi_value*omega*omega;
    if (!decay.valid || !finite(denom) || denom <= 0.0)
        return {0.0,false};
    const double value = power*decay.value/denom;
    return {value,finite(value)};
}
} // namespace swe::sigma_form
#endif // SWE_SIGMA_FORM_GEOMETRY_HPP
