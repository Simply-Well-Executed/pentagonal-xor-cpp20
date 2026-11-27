// SPDX-License-Identifier: 0BSD
#ifndef LF9WN9Z_HPP
#define LF9WN9Z_HPP
#if defined(LF9WN9Z_BUILD)
#define LF9WN9Z_API __declspec(dllexport)
#else
#define LF9WN9Z_API __declspec(dllimport)
#endif
// State array: x,y,z (micrometers), vy,vz (micrometers per 10ms tick).
// Caller owns five writable long long elements. Return 1 success, 0 rejection.
extern "C" {
LF9WN9Z_API int lf9wn9z_reset(long long* state, unsigned count, unsigned ceiling) noexcept;
LF9WN9Z_API int lf9wn9z_action(long long* state, unsigned count, unsigned action) noexcept;
LF9WN9Z_API int lf9wn9z_step(long long* state, unsigned count, unsigned ticks) noexcept;
}
#undef LF9WN9Z_API
#endif
