
#include <array>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <random>

namespace dsv {
namespace mod {

// ──────────────────────────────────────────────────────────────────────────────
// SIR_Binomial — Cadena Binomial Estocástica en Tiempo Discreto
//
//   S_{n+1} = S_n - ε_1
//   I_{n+1} = I_n - ε_2 + ε_1
//   R_{n+1} = R_n + ε_2
//
//   ε_1 ~ Binomial(S_n, q_n)   (nuevos infectados en el paso n)
//   ε_2 ~ Binomial(I_n, p_n)   (nuevos recuperados en el paso n)
// ──────────────────────────────────────────────────────────────────────────────
template<typename Type = int>
struct SIR_Binomial_Model {

    using state_type = Type;
    using time_type  = int;

    static constexpr bool   is_discrete = true;
    static constexpr size_t dim         = 3; // S, I, R
    static constexpr size_t noise_dim   = 2; // ε_1 (infección), ε_2 (recuperación)

    enum { S, I, R };

    // Parámetros epidemiológicos configurables desde la UI
    double beta_0  = 0.32;   // tasa base de transmisión
    double alpha   = 0.15;   // amplitud estacional [0, 1)
    double periodo = 365.0;  // período estacional en pasos (días)

    double gamma_0 = 0.08;   // tasa base de recuperación
    double delta   = 0.50;   // mejora máxima de recuperación con el tiempo
    double tau     = 60.0;   // tiempo característico de adaptación

    // Probabilidad q_n de que un susceptible se infecte en el paso n
    double prob_infeccion(const std::array<state_type, dim>& x, time_type n) const {
        double N = static_cast<double>(x[S] + x[I] + x[R]);
        if (N <= 0.0 || x[I] <= 0) return 0.0;

        constexpr double two_pi = 6.28318530717958647692;
        double beta_n = beta_0 * (1.0 + alpha * std::cos(two_pi * n / periodo));
        beta_n = std::max(0.0, beta_n);

        double q = 1.0 - std::exp(-beta_n * (static_cast<double>(x[I]) / N));
        return std::clamp(q, 0.0, 1.0);
    }

    // Probabilidad p_n de que un infectado se recupere en el paso n
    double prob_recuperacion(const std::array<state_type, dim>& x, time_type n) const {
        if (x[I] <= 0) return 0.0;

        double factor_n = static_cast<double>(n) / (static_cast<double>(n) + tau);
        double gamma_n  = std::max(0.0, gamma_0 * (1.0 + delta * factor_n));

        double p = 1.0 - std::exp(-gamma_n);
        return std::clamp(p, 0.0, 1.0);
    }

    // Parte determinista: mantiene el estado base X_n
    //    (el integrador hará: X_{n+1} = drift(X_n, n) + diffusion(X_n, n, eps))
    void drift(const std::array<state_type, dim>& x,
               time_type /*n*/,
               std::array<state_type, dim>& out) const
    {
        out = x;
    }

    // 2. Generación de las variables aleatorias binomiales ε_1 y ε_2
    void epsilon(const std::array<state_type, dim>& x,
                 time_type n,
                 std::mt19937& rng,
                 std::array<state_type, noise_dim>& eps_out) const
    {
        eps_out = {}; // Limpiar matriz

        int ensayos_S = std::max(0, static_cast<int>(x[S]));
        int ensayos_I = std::max(0, static_cast<int>(x[I]));

        double q_n = prob_infeccion(x, n);
        double p_n = prob_recuperacion(x, n);

        std::binomial_distribution<int> dist_inf(ensayos_S, q_n);
        std::binomial_distribution<int> dist_rec(ensayos_I, p_n);

        // Guardamos ε_1 en [0][0] y ε_2 en [0][1]
        eps_out[0] = static_cast<state_type>(dist_inf(rng)); // ε_1
        eps_out[1] = static_cast<state_type>(dist_rec(rng)); // ε_2
    }

    // 3. Flujo neto entre compartimentos: [-ε_1, +ε_1 - ε_2, +ε_2]
    void diffusion(const std::array<state_type, dim>& /*x*/,
                   time_type /*n*/,
                   std::array<state_type, noise_dim>& eps,
                   std::array<state_type, dim>& out) const
    {
        state_type eps_1 = eps[0];
        state_type eps_2 = eps[1];

        out[S] = -eps_1;
        out[I] =  eps_1 - eps_2;
        out[R] =  eps_2;
    }
};

} // namespace mod
} // namespace dsv