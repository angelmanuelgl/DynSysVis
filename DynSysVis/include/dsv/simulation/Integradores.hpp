#ifndef INTEGRADORES_HPP
#define INTEGRADORES_HPP

#include <random>
#include "dsv/core/Logger.hpp"




namespace dsv {
namespace sim {

inline std::mt19937& default_rng() {
    static thread_local std::mt19937 gen(std::random_device{}());
    return gen;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// tiempo continuo: EDO y EDE
// Ecuacion Diferenciales Ordinarias (ODE) y Estocásticas (SDE)
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // --- --- ODE --- --- 
    // EULER
    template<typename Instance>
    void E_step(Instance& sys, float dt ){
        using Model = typename std::decay<decltype(sys.getModel())>::type;
        constexpr size_t d = Model::dim;

        std::array<float, d> a;
        sys.getModel().drift(sys.state, sys.t, a); // Solo necesitamos la tendencia

        for(size_t i = 0; i < d; ++i ){
            sys.state[i] += a[i] * dt;
        }
        sys.t += dt;
    }

    // --- --- SDE --- --- 
    //  Euler-Maruyama un ruido
    template<typename Instance>
    void EM_step_simple(Instance& sys, typename Instance::time_type dt, std::mt19937& rng = default_rng()) {
        using Model = typename Instance::model_type;
        using Real  = typename Instance::state_type;
        constexpr size_t d = Model::dim;

        std::array<float, d> a;
        std::array<float, d> g; // Vector de difusión en lugar de matriz

        sys.getModel().drift(sys.state, sys.t, a);
        sys.getModel().diffusion(sys.state, sys.t, g);

        static std::mt19937 gen(std::random_device{}());
        std::normal_distribution<Real> dist(Real(0), Real(1));
        float dW = dist(gen) * std::sqrt(dt);

        for(size_t i = 0; i < d; ++i) {
            sys.state[i] += a[i] * dt + g[i] * dW;
        }
        sys.t += dt;
    }

    //  Euler-Maruyama un m ruidos Independiente
    template<typename Instance>
    void EM_step(Instance& sys, float dt, std::mt19937& rng = default_rng() )
    {
        using Model = typename Instance::model_type;
        using Real  = typename Instance::state_type;

        constexpr size_t d = Model::dim;
        constexpr size_t m = Model::noise_dim;

        std::array<float, d> a;
        std::array<std::array<float, m>, d> B;
        std::array<float, m> dW;

        sys.getModel().drift(sys.state, sys.t, a);
        sys.getModel().diffusion(sys.state, sys.t, B);

        static std::mt19937 gen(std::random_device{}());
        static std::normal_distribution<Real> dist(Real(0), Real(1));

        for(size_t k = 0; k < m; ++k)
            dW[k] = dist(gen) * std::sqrt(dt);

        for(size_t i = 0; i < d; ++i)
        {
            Real stochastic = Real(0);
            for(size_t k = 0; k < m; ++k)
                stochastic += B[i][k] * dW[k];

            sys.state[i] += a[i] * dt + stochastic;
        }

        sys.t += dt;
    }

    // GENERAL // Para modelos CONTINUOS (EDOs y SDEs): requiere dt
    template<typename Instance>
    void step(Instance& sys, typename Instance::time_type dt, std::mt19937& rng = default_rng() )
    {
        using Model = typename std::decay_t<decltype(sys.getModel())>;

        if constexpr (Model::noise_dim == 0) {
            E_step(sys, dt); // O RK4_step(sys, dt)
        }
        else if constexpr (Model::noise_dim == 1) {
            EM_step_simple(sys, dt, rng);
        }
        else {
            EM_step(sys, dt, rng);
        }
    }

    // --- --- ODE --- --- 
    // Runge-Kutta 4to Orden (RK4)
    template<typename Instance>
    void RK4_step(Instance& sys, typename Instance::time_type dt )
    {
        using Model = typename Instance::model_type;
        using Real  = typename Instance::state_type;
        using Time  = typename Instance::time_type;
        constexpr size_t d = Model::dim;

        // Estructuras para las 4 etapas
        std::array<Real, d> k1, k2, k3, k4;
        std::array<Real, d> temp_state;

        const Time h  = dt;
        const Time h2 = h / Time(2);
        const Time h6 = h / Time(6);

        // k1 = f(t, x)
        sys.getModel().drift(sys.state, sys.t, k1);

        // k2 = f(t + h/2, x + h/2 * k1)
        for(size_t i = 0; i < d; ++i) 
            temp_state[i] = sys.state[i] + static_cast<Real>(h2)* k1[i];
        sys.getModel().drift(temp_state, sys.t + h2, k2);

        // k3 = f(t + h/2, x + h/2 * k2)
        for(size_t i = 0; i < d; ++i) 
            temp_state[i] = sys.state[i] + static_cast<Real>(h2) * k2[i];
        sys.getModel().drift(temp_state, sys.t + h2, k3);

        // k4 = f(t + h, x + h * k3)
        for(size_t i = 0; i < d; ++i)
             temp_state[i] = sys.state[i] + static_cast<Real>(h) * k3[i];
        sys.getModel().drift(temp_state, sys.t + h, k4);

        // actualizacion final: x = x + (h/6) * (k1 + 2k2 + 2k3 + k4)
        for(size_t i = 0; i < d; ++i) {
            sys.state[i] += h6 * (k1[i] + Real(2) * k2[i] + Real(2) * k3[i] + k4[i]);
        }
        
        sys.t += h;
    }


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// tiempo discreto
// Ecuacion en Diferencias (Ec en Dif) y Estocásticas (Ec en Dif Estocásticas)
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // ecuaciones en diferencias
    template<typename Instance>
    void EcEnDif_step(Instance& sys) {
        using Model = typename Instance::model_type;
        using State = typename Instance::state_type;
        constexpr size_t d = Model::dim;

        // Importante: escribir en 'next' para no sobrescribir sys.state a mitad del cálculo
        std::array<State, d> next;
        sys.getModel().drift(sys.state, sys.t, next);

        sys.state = next;
        sys.t += 1;
    }

    // Ecuacion en dIferencias Estocastica
    //    x_{n+1} = drift(x_n, n) + diffusion(x_n, n, epsilon(x_n, n))
    template<typename Instance>
    void EcEnDifEstoc_step(Instance& sys, std::mt19937& rng = default_rng()) {
        using Model = typename Instance::model_type;
        using State = typename Instance::state_type;
        constexpr size_t d = Model::dim;
        constexpr size_t m = Model::noise_dim;

        std::array<State, d> det_part;
        std::array<std::array<State, m>, d> eps;
        std::array<State, d> stoch_part;

        const auto& model = sys.getModel();
        model.drift(sys.state, sys.t, det_part);
        model.epsilon(sys.state, sys.t, rng, eps);
        model.diffusion(sys.state, sys.t, eps, stoch_part);

        for (size_t i = 0; i < d; ++i)
            sys.state[i] = det_part[i] + stoch_part[i];

        sys.t += 1;
    }

    // general: para modelos discretos (Ec en Dif y Ec en Dif Estocásticas): no requiere dt
    template<typename Instance>
    void step(Instance& sys, std::mt19937& rng = default_rng()) {
        using Model = typename Instance::model_type;
        
        if constexpr (Model::noise_dim == 0) {
            EcEnDif_step(sys);
        } else {
            EcEnDifEstoc_step(sys, rng);
        }
    }


} // namespace sim
} // namespace dsv
#endif // integradores