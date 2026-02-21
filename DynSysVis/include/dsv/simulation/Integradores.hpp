#ifndef INTEGRADORES_HPP
#define INTEGRADORES_HPP

#include <random>
#include "dsv/core/Logger.hpp"

namespace dsv {
namespace sim {
    // --- --- ODE --- --- 
    // EULER
    template<typename Instance>
    void E_step(Instance& sys, float dt) {
        using Model = decltype(sys.model);
        constexpr size_t d = Model::dim;

        std::array<float, d> a;
        sys.model.drift(sys.state, sys.t, a); // Solo necesitamos la tendencia

        for(size_t i = 0; i < d; ++i ){
            sys.state[i] += a[i] * dt;
        }
        sys.t += dt;
    }

    // --- --- SDE --- --- 
    //  Euler-Maruyama un ruido
    template<typename Instance>
    void EM_step_simple(Instance& sys, float dt) {
        using Model = decltype(sys.model);
        constexpr size_t d = Model::dim;

        std::array<float, d> a;
        std::array<float, d> g; // Vector de difusión en lugar de matriz

        sys.model.drift(sys.state, sys.t, a);
        sys.model.diffusion(sys.state, sys.t, g);

        static std::mt19937 gen(std::random_device{}());
        static std::normal_distribution<float> dist(0.0f, 1.0f);
        float dW = dist(gen) * std::sqrt(dt);

        for(size_t i = 0; i < d; ++i) {
            sys.state[i] += a[i] * dt + g[i] * dW;
        }
        sys.t += dt;
    }

    //  Euler-Maruyama un m ruidos Independiente
    template<typename Instance>
    void EM_step(Instance& sys, float dt)
    {
        using Model = decltype(sys.model);

        constexpr size_t d = Model::dim;
        constexpr size_t m = Model::noise_dim;

        std::array<float, d> a;
        std::array<std::array<float, m>, d> B;
        std::array<float, m> dW;

        sys.model.drift(sys.state, sys.t, a);
        sys.model.diffusion(sys.state, sys.t, B);

        static std::mt19937 gen(std::random_device{}());
        static std::normal_distribution<float> dist(0.0f, 1.0f);

        for(size_t k = 0; k < m; ++k)
            dW[k] = dist(gen) * std::sqrt(dt);

        for(size_t i = 0; i < d; ++i)
        {
            float stochastic = 0.0f;
            for(size_t k = 0; k < m; ++k)
                stochastic += B[i][k] * dW[k];

            sys.state[i] += a[i] * dt + stochastic;
        }

        sys.t += dt;
    }

    // GENERAL
    template<typename Instance>
    void step(Instance& sys, float dt) {
        using Model = decltype(sys.model);
        
        // dim = 0//  determinista
        if constexpr (Model::noise_dim == 0) {
            E_step(sys, dt);
        } 
        // dim = 1// simple
        else if constexpr (Model::noise_dim == 1) {
            EM_step_simple(sys, dt);
        }
        // n x m matriz
        else {
            EM_step(sys, dt); // Tu código original de matriz
        }
}

} // namespace sim
} // namespace dsv
#endif // integradores