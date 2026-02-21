#ifndef ORG_HPP
#define ORG_HPP

#include <array>
#include <cstddef>

namespace dsv {
namespace mod {

struct ORG_Model {

    static constexpr size_t dim = 3;
    static constexpr size_t noise_dim = 0;

    enum { O, G, R };

    float bO = 0.05f;
    float betaOG = 4.0f;
    float betaGR = 2.5f;
    float betaRG = 1.2f;
    float kG = 50.0f;
    float kR = 50.0f;
    float dG = 0.005f;
    float dR = 0.005f;

    float ruidoO = 0.01f;
    float ruidoG = 0.01f;
    float ruidoR = 0.01f;

    void drift(const std::array<float, dim>& v,
               float t,
               std::array<float, dim>& out) const
    {
        out[O] = (bO * v[O]) - (betaOG / (kG + v[G])) * v[O];

        out[G] = (betaOG / (kG + v[G])) * v[O]
                 - (betaGR / (kR + v[R])) * v[G]
                 - (dG * v[G])
                 + (betaRG / (kG + v[G])) * v[R];

        out[R] = (betaGR / (kR + v[R])) * v[G]
                 - (betaRG / (kG + v[G])) * v[R]
                 - (dR * v[R]);
    }

};

struct ORG_Instance {
    ORG_Model model;

    std::array<float, ORG_Model::dim> state { 10.0f, 1.0f, 0.0f };
    float t = 0.0f;
};



}  // end dsv
} // end sim
#endif // end mod
