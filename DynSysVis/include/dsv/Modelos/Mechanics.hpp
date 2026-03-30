#ifndef MECANICA_HPP
#define MECANICA_HPP

#include <array>
#include <cmath>

namespace dsv {
namespace mod {

// ──────────────────────────────────────────────────────────────────────────────
// MassSpringDamper — Resorte Masa-Amortiguador
//
//   dq/dt = p
//   dp/dt = -(k/m)*q - (c/m)*p
// ──────────────────────────────────────────────────────────────────────────────
struct MassSpringDamper_Model {
    static constexpr size_t dim       = 2;
    static constexpr size_t noise_dim = 0;

    float m = 1.0f; // Masa
    float k = 1.0f; // Constante elástica
    float c = 0.1f; // Amortiguamiento (c=0 para oscilación pura)

    enum { Q, P }; // Posición, Velocidad

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        out[Q] = x[P];
        out[P] = -(k / m) * x[Q] - (c / m) * x[P];
    }
};

// ──────────────────────────────────────────────────────────────────────────────
// Pendulum — Péndulo Simple Amortiguado
//
//   dtheta/dt = omega
//   domega/dt = -(g/L)*sin(theta) - gamma*omega
// ──────────────────────────────────────────────────────────────────────────────
struct Pendulum_Model {
    static constexpr size_t dim       = 2;
    static constexpr size_t noise_dim = 0;

    float g     = 9.81f; // Gravedad
    float L     = 1.0f;  // Longitud
    float gamma = 0.1f;  // Fricción

    enum { THETA, OMEGA };

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        out[THETA] = x[OMEGA];
        out[OMEGA] = -(g / L) * std::sin(x[THETA]) - gamma * x[OMEGA];
    }
};

// ──────────────────────────────────────────────────────────────────────────────
// DoublePendulum — Péndulo Doble (Caos Mecánico)
//
// Ecuaciones derivadas de Euler-Lagrange para masas m1, m2 y longitudes l1, l2.
// ──────────────────────────────────────────────────────────────────────────────
struct DoublePendulum_Model {
    static constexpr size_t dim       = 4;
    static constexpr size_t noise_dim = 0;

    float m1 = 1.0f;
    float m2 = 1.0f;
    float l1 = 1.0f;
    float l2 = 1.0f;
    float g  = 9.81f;

    enum { T1, T2, W1, W2 }; // Ángulos y velocidades angulares

    void drift(const std::array<float, dim>& x, float /*t*/, std::array<float, dim>& out) const {
        float delta = x[T2] - x[T1];
        float den1 = (2*m1 + m2 - m2 * std::cos(2*delta));
        // float den2 = (l2/l1) * den1;

        out[T1] = x[W1];
        out[T2] = x[W2];

        // Aceleración angular 1
        out[W1] = (-g*(2*m1 + m2)*std::sin(x[T1]) - m2*g*std::sin(x[T1] - 2*x[T2]) 
                  - 2*std::sin(delta)*m2*(x[W2]*x[W2]*l2 + x[W1]*x[W1]*l1*std::cos(delta))) 
                  / (l1 * den1);

        // Aceleración angular 2
        out[W2] = (2*std::sin(delta)*(x[W1]*x[W1]*l1*(m1 + m2) 
                  + g*(m1 + m2)*std::cos(x[T1]) + x[W2]*x[W2]*l2*m2*std::cos(delta))) 
                  / (l2 * den1);
    }
};

} // namespace mod
} // namespace dsv
#endif