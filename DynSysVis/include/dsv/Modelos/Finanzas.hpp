#ifndef FINANZAS_HPP
#define FINANZAS_HPP

#include <array>
#include <cmath>
#include <cstddef>

// Modelos de difusión usados en finanzas matemáticas.
// En todos los casos el ruido modela incertidumbre de mercado.
//
// Convención de difusión para m=1:
//   El integrador EM_step_simple espera que diffusion() llene
//   un std::array<float, d> donde cada elemento es el coeficiente
//   g_i tal que  dxi = fi·dt + gi·dW.

namespace dsv {
namespace mod {


// ──────────────────────────────────────────────────────────────────────────────
// DriftDiffusion — Movimiento Browniano con Deriva
//
//   dx = μ dt + σ dW
//
// Solución exacta: x(t) = x0 + μt + σ·W(t)
// Base de casi toda la teoría de opciones. El ruido es aditivo
// (independiente del estado), por lo que tanto Euler-Maruyama
// como RK4 son exactos en distribución para este modelo.
// ──────────────────────────────────────────────────────────────────────────────
struct DriftDiffusion_Model {

    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;

    float mu    = 0.05f; // deriva (drift): tendencia determinista
    float sigma = 0.20f; // volatilidad: intensidad del ruido

    void drift(const std::array<float, dim>& /*x*/,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        out[0] = mu;
    }

    void diffusion(const std::array<float, dim>& /*x*/,
                   float /*t*/,
                   std::array<float, dim>& out) const
    {
        out[0] = sigma; // ruido aditivo, independiente del estado
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// BlackScholes — Movimiento Browniano Geométrico (GBM)
//
//   dx = μ x dt + σ x dW
//
// Solución exacta: x(t) = x0 · exp((μ - σ²/2)t + σ·W(t))
// El precio nunca es negativo (si x0 > 0). El ruido es multiplicativo:
// a mayor precio, mayor impacto absoluto de la volatilidad.
// μ - σ²/2 es la tasa de crecimiento efectiva (corrección de Itô).
// ──────────────────────────────────────────────────────────────────────────────
struct BlackScholes_Model {

    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;

    float mu    = 0.08f; // tasa de retorno esperada (anualizada)
    float sigma = 0.20f; // volatilidad porcentual (anualizada)

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        out[0] = mu * x[0]; // crecimiento proporcional al precio
    }

    void diffusion(const std::array<float, dim>& x,
                   float /*t*/,
                   std::array<float, dim>& out) const
    {
        out[0] = sigma * x[0]; // ruido multiplicativo: escala con el precio
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// CIR — Cox-Ingersoll-Ross (Tasa de Interés)
//
//   dx = a(b - x) dt + σ √x dW
//
// Modela tasas de interés de corto plazo. La raíz cuadrada en la difusión
// asegura que x=0 es una barrera: la tasa no puede volverse negativa
// (siempre que 2ab ≥ σ², condición de Feller).
//
// El proceso revierte a la media b con velocidad a.
// ──────────────────────────────────────────────────────────────────────────────
struct CIR_Model {

    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;

    float a     = 0.5f;  // velocidad de reversión a la media
    float b     = 0.05f; // media de largo plazo (tasa objetivo)
    float sigma = 0.10f; // volatilidad

    // Condición de Feller: 2*a*b >= sigma^2 garantiza x > 0
    // Con los defaults: 2*0.5*0.05 = 0.05 >= 0.01 = 0.1^2 ✓

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        out[0] = a * (b - x[0]); // atracción hacia la media b
    }

    void diffusion(const std::array<float, dim>& x,
                   float /*t*/,
                   std::array<float, dim>& out) const
    {
        // Se clampea en 0 para robustez numérica cuando x roza 0
        out[0] = sigma * std::sqrt(std::max(0.0f, x[0]));
    }
};


} // namespace mod
} // namespace dsv
#endif // FINANZAS_HPP
