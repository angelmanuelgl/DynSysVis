#ifndef FISICA_ESTADISTICA_HPP
#define FISICA_ESTADISTICA_HPP

#include <array>
#include <cstddef>

// Modelos de física estadística y termodinámica.
// El ruido modela fluctuaciones térmicas del entorno.
//
// Convención de difusión para m=1:
//   diffusion() llena std::array<float, d> con el coeficiente g_i
//   tal que  dxi = fi·dt + gi·dW.

namespace dsv {
namespace mod {


// ──────────────────────────────────────────────────────────────────────────────
// OrnsteinUhlenbeck — Proceso de Ornstein-Uhlenbeck (Langevin con fricción)
//
//   dx = -γ x dt + √(2D) dW
//
// Describe la velocidad de una partícula browniana masiva con fricción γ.
// El proceso es gaussiano y revierte a la media 0.
//
// Solución exacta: x(t) = x0·e^(-γt) + √(D/γ)·(proceso gaussiano estacionario)
// Distribución estacionaria: N(0, D/γ)
//
// Parámetro    Fenómeno
//   γ          Coeficiente de fricción viscosa (amortiguamiento)
//   D          Coeficiente de difusión térmica (∝ temperatura por fluctuation-dissipation)
// ──────────────────────────────────────────────────────────────────────────────
struct OrnsteinUhlenbeck_Model {

    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;

    float gamma = 1.0f;  // coeficiente de fricción         (γ > 0)
    float D     = 0.5f;  // coeficiente de difusión térmica (D > 0)

    // Relación de fluctuación-disipación: D = kT/m
    // Con γ=1, D=0.5 → varianza estacionaria = D/γ = 0.5

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        out[0] = -gamma * x[0]; // fricción lineal: empuja hacia x=0
    }

    void diffusion(const std::array<float, dim>& /*x*/,
                   float /*t*/,
                   std::array<float, dim>& out) const
    {
        out[0] = std::sqrt(2.0f * D); // ruido térmico constante
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// DoubleWell — Potencial de Doble Pozo (Kramers / Transiciones de Fase)
//
//   dx = (x - x³) dt + σ dW
//
// El drift f(x) = x - x³ = -dV/dx  es el negativo del gradiente del potencial:
//   V(x) = -x²/2 + x⁴/4   (dos pozos en x=±1, barrera en x=0)
//
// Puntos fijos:
//   x* = ±1  estables   (fondo de los pozos)
//   x* =  0  inestable  (cima de la barrera)
//
// El ruido σ permite "saltos de Kramers" entre los dos estados estables.
// A mayor σ, más frecuentes las transiciones.
// Tasa de Kramers (aprox): k ~ exp(-ΔV/σ²) donde ΔV = 1/4.
// ──────────────────────────────────────────────────────────────────────────────
struct DoubleWell_Model {

    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;

    float sigma = 0.30f; // intensidad del ruido (σ > 0)
                         // σ < 0.5: saltos raros; σ > 0.7: mezcla frecuente

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        out[0] = x[0] - x[0] * x[0] * x[0]; // f(x) = x - x³
    }

    void diffusion(const std::array<float, dim>& /*x*/,
                   float /*t*/,
                   std::array<float, dim>& out) const
    {
        out[0] = sigma; // ruido aditivo constante
    }
};


} // namespace mod
} // namespace dsv
#endif // FISICA_ESTADISTICA_HPP
