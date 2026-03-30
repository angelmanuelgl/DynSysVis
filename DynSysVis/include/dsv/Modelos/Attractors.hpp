#ifndef ATTRACTORES_HPP
#define ATTRACTORES_HPP

#include <array>
#include <cmath>

namespace dsv {
namespace mod {

// ──────────────────────────────────────────────────────────────────────────────
// Lorenz — El Atractor de la Mariposa
// ──────────────────────────────────────────────────────────────────────────────
struct Lorenz_Model {
    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    float sigma = 10.0f;
    float rho   = 28.0f;
    float beta  = 8.0f / 3.0f;

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        out[0] = sigma * (x[1] - x[0]);
        out[1] = x[0] * (rho - x[2]) - x[1];
        out[2] = x[0] * x[1] - beta * x[2];
    }
};

// ──────────────────────────────────────────────────────────────────────────────
// Rossler — Atractor de una sola no-linealidad
// ──────────────────────────────────────────────────────────────────────────────
struct Rossler_Model {
    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    float a = 0.1f;
    float b = 0.1f;
    float c = 14.0f;

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        out[0] = -x[1] - x[2];
        out[1] = x[0] + a * x[1];
        out[2] = b + x[2] * (x[0] - c);
    }
};

// ──────────────────────────────────────────────────────────────────────────────
// Chua — Dinámica de Circuitos (Diodo de Chua)
// ──────────────────────────────────────────────────────────────────────────────
struct Chua_Model {
    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    float alpha = 15.6f;
    float beta  = 28.0f;
    float m0    = -1.143f;
    float m1    = -0.714f;

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        // Función no lineal h(x)
        float h = m1 * x[0] + 0.5f * (m0 - m1) * (std::abs(x[0] + 1.0f) - std::abs(x[0] - 1.0f));

        out[0] = alpha * (x[1] - x[0] - h);
        out[1] = x[0] - x[1] + x[2];
        out[2] = -beta * x[1];
    }
};

} // namespace mod
} // namespace dsv
#endif