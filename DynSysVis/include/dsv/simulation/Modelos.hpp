#ifdef MODELOS_HPP
#define MODELOS_HPP


namespace dsv::mdelso
{
// 1. Definición del modelo (Estructura "Header-only" o similar)
struct ModeloSIR {
    // Parámetros que puedes cambiar en tiempo real
    float beta = 0.3f;
    float gamma = 0.1f;
    float sigma_ruido = 0.01f;

    // Término de avance temporal (dt)
    void evolucion(const std::vector<float>& x, float t, std::vector<float>& out) {
        // Mapeo semántico rápido
        float S = x[0], I = x[1], R = x[2];
        
        out[0] = -beta * S * I;          // dS
        out[1] = beta * S * I - gamma * I; // dI
        out[2] = gamma * I;              // dR
    }

    // Término estocástico (dW)
    void fluctuacion(const std::vector<float>& x, float t, std::vector<float>& out) {
        out[0] = -sigma_ruido * x[0];
        out[1] = sigma_ruido * x[1];
        out[2] = 0;
    }
    
    size_t get_dims() { return 3; }
};

// 2. El Solucionador (Genérico)
template <typename T_Modelo>
void nextStep(T_Modelo& modelo, std::vector<float>& x, float& t, float dt) {
    size_t dims = modelo.get_dims();
    std::vector<float> f(dims), g(dims);

    modelo.evolucion(x, t, f);
    modelo.fluctuacion(x, t, g);

    float dW = generarRuido() * std::sqrt(dt);

    for(size_t i = 0; i < dims; ++i) {
        x[i] += f[i] * dt + g[i] * dW;
    }
    t += dt;
}



struct Colonia {
    float O, G, R, t;
    std::string id;
    sf::Color color;

    void actualizar(float dt ){
        // Constantes del modelo
        const float bO = 0.05f, betaOG = 4.0f, betaGR = 2.5f;
        const float betaRG = 1.2f, kG = 50.0f, kR = 50.0f, dG = 0.005f, dR = 0.005f;

        float dO = (bO * O) - (betaOG / (kG + G)) * O;
        float dG_dt = (betaOG / (kG + G)) * O - (betaGR / (kR + R)) * G - (dG * G) + (betaRG / (kG + G)) * R;
        float dR_dt = (betaGR / (kR + R)) * G - (betaRG / (kG + G)) * R - (dR * R);

        O += dO * dt;
        G += dG_dt * dt;
        R += dR_dt * dt;
        t += dt;
    }
};

} // namespace dsv
#endif