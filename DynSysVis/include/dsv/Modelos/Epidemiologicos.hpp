#ifndef EPIDEMIOLOGICOS_HPP
#define EPIDEMIOLOGICOS_HPP

#include <array>
#include <cstddef>

// Familia de modelos compartimentales epidemiológicos.
// Todos son deterministas por ahora (noise_dim = 0).
//
// Convención de estado:
//   N = suma de todos los compartimentos vivos (excluye D en SEIARD).
//   Los índices de compartimento se exponen como enum para legibilidad.

namespace dsv {
namespace mod {


// ──────────────────────────────────────────────────────────────────────────────
// SIS — Susceptible → Infectado → Susceptible
//
// Para enfermedades sin inmunidad permanente (resfriado común, gonorrea).
// Los recuperados vuelven directamente al compartimento S.
//
//   dS/dt = -β·S·I/N + γ·I
//   dI/dt =  β·S·I/N - γ·I
//
// Umbral epidémico: R0 = β/γ
//   R0 > 1 → endemia estable;  R0 < 1 → extinción de I.
// ──────────────────────────────────────────────────────────────────────────────
struct SIS_Model {

    static constexpr size_t dim       = 2;
    static constexpr size_t noise_dim = 0;

    float beta  = 0.3f;  // tasa de contacto efectivo
    float gamma = 0.05f; // tasa de recuperación

    enum { S, I };

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        float N = x[S] + x[I];
        float infeccion = beta * x[S] * x[I] / N;

        out[S] = -infeccion + gamma * x[I];
        out[I] =  infeccion - gamma * x[I];
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// SIR — Susceptible → Infectado → Recuperado
//
// El modelo estándar. Inmunidad permanente tras recuperación.
//
//   dS/dt = -β·S·I/N
//   dI/dt =  β·S·I/N - γ·I
//   dR/dt =  γ·I
//
// Umbral epidémico: R0 = β/γ
//   R0 > 1 → epidemia; la curva I sube, alcanza su pico y decae.
// ──────────────────────────────────────────────────────────────────────────────
struct SIR_Model {

    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    float beta  = 0.3f;  // tasa de transmisión
    float gamma = 0.05f; // tasa de recuperación

    enum { S, I, R };

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        float N = x[S] + x[I] + x[R];
        float infeccion = beta * x[S] * x[I] / N;

        out[S] = -infeccion;
        out[I] =  infeccion - gamma * x[I];
        out[R] =  gamma * x[I];
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// SEIR — Susceptible → Expuesto → Infectado → Recuperado
//
// Añade un período de incubación: los Expuestos están infectados
// pero aún no son contagiosos.
//
//   dS/dt = -β·S·I/N
//   dE/dt =  β·S·I/N - ε·E
//   dI/dt =  ε·E     - γ·I
//   dR/dt =  γ·I
//
// El retraso ε suaviza y retrasa el pico de I respecto al SIR puro.
// 1/ε = tiempo promedio de incubación en días.
// ──────────────────────────────────────────────────────────────────────────────
struct SEIR_Model {

    static constexpr size_t dim       = 4;
    static constexpr size_t noise_dim = 0;

    float beta    = 0.3f;         // tasa de transmisión
    float gamma   = 0.05f;        // tasa de recuperación
    float epsilon = 1.0f / 5.0f;  // tasa de incubación (período = 5 días)

    enum { S, E, I, R };

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        float N = x[S] + x[E] + x[I] + x[R];
        float infeccion = beta * x[S] * x[I] / N;

        out[S] = -infeccion;
        out[E] =  infeccion       - epsilon * x[E];
        out[I] =  epsilon * x[E]  - gamma   * x[I];
        out[R] =  gamma   * x[I];
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// SIRS — Susceptible → Infectado → Recuperado → Susceptible
//
// Inmunidad temporal: los Recuperados pierden la inmunidad a tasa ξ
// y regresan a Susceptibles. Puede generar ondas epidémicas recurrentes.
//
//   dS/dt = -β·S·I/N + ξ·R
//   dI/dt =  β·S·I/N - γ·I
//   dR/dt =  γ·I     - ξ·R
//
// 1/ξ = duración media de la inmunidad en días.
// ──────────────────────────────────────────────────────────────────────────────
struct SIRS_Model {

    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    float beta  = 0.3f;         // tasa de transmisión
    float gamma = 0.05f;        // tasa de recuperación
    float xi    = 1.0f / 180.f; // tasa de pérdida de inmunidad (~6 meses)

    enum { S, I, R };

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        float N = x[S] + x[I] + x[R];
        float infeccion = beta * x[S] * x[I] / N;

        out[S] = -infeccion          + xi    * x[R];
        out[I] =  infeccion          - gamma * x[I];
        out[R] =  gamma   * x[I]     - xi    * x[R];
    }
};


// ──────────────────────────────────────────────────────────────────────────────
// SEIARD — Modelo de Pandemia Completo
//
// Extiende SEIR con Asintomáticos (A) y Defunciones (D).
// El compartimento D es absorbente: no sale de él.
// N se calcula solo sobre los compartimentos vivos (excluye D).
//
//   dS/dt = -S·(β_I·I + β_A·A) / N
//   dE/dt =  S·(β_I·I + β_A·A) / N  - ε·E
//   dI/dt =  p·ε·E                   - (γ_I + μ)·I
//   dA/dt =  (1-p)·ε·E               - γ_A·A
//   dR/dt =  γ_I·I + γ_A·A
//   dD/dt =  μ·I
//
// ──────────────────────────────────────────────────────────────────────────────
struct SEIARD_Model {

    static constexpr size_t dim       = 6;
    static constexpr size_t noise_dim = 0;

    float beta_I   = 0.30f;        // infectividad de sintomáticos
    float beta_A   = 0.15f;        // infectividad de asintomáticos (menor)
    float epsilon  = 1.0f / 5.0f;  // tasa de incubación (período = 5 días)
    float p        = 0.60f;        // fracción que desarrolla síntomas [0,1]
    float gamma_I  = 0.05f;        // tasa de recuperación de sintomáticos
    float gamma_A  = 0.08f;        // tasa de recuperación de asintomáticos
    float mu       = 0.005f;       // tasa de mortalidad por enfermedad

    enum { S, E, I, A, R, D };

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const
    {
        // N excluye muertos: la dinámica ocurre sobre los vivos
        float N = x[S] + x[E] + x[I] + x[A] + x[R];

        float fuerza_infeccion = (beta_I * x[I] + beta_A * x[A]) / N;
        float nuevas_exposiciones = x[S] * fuerza_infeccion;

        out[S] = -nuevas_exposiciones;
        out[E] =  nuevas_exposiciones          - epsilon          * x[E];
        out[I] =  p       * epsilon * x[E]     - (gamma_I + mu)  * x[I];
        out[A] =  (1.0f-p)* epsilon * x[E]     - gamma_A         * x[A];
        out[R] =  gamma_I * x[I]               + gamma_A         * x[A];
        out[D] =  mu      * x[I];
    }
};


} // namespace mod
} // namespace dsv
#endif // EPIDEMIOLOGICOS_HPP
