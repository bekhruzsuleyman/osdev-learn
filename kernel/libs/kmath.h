#ifndef KMATH_H
#define KMATH_H

#include <stdint.h>

static inline int64_t kround_f32(float num) {
    return (num < 0.0f)
        ? (int64_t) (num - 0.5f)
        : (int64_t) (num + 0.5f);
}

static inline int64_t kround_f64(double num) {
    return (num < 0.0)
        ? (int64_t) (num - 0.5)
        : (int64_t) (num + 0.5);
}

static inline uint64_t kabs_int64(int64_t x) {
    return (x < 0)
            ? (uint64_t) (-(x + 1)) + 1
            : (uint64_t) x;
}

static inline int64_t kpower(int64_t x, uint64_t n) {
    int64_t y = 1;

    for (uint64_t i = 0; i < n; i++) y = y * x;

    return y;
}

#endif