#ifndef MOD_HPP
#define MOD_HPP

#include <array>
#include <variant>





/* modelo tiempo continuo generico:
struct Model {
    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;
    void drift(const std::array<float, dim>& x,
               float t,
               std::array<float, dim>& out) const
    {   
    }
    void diffusion(const std::array<float, dim>& x,
                   float t,
                   std::array<float, dim>& out) const
    { 
    }
    }
    void diffusion(const std::array<float, dim>& x,
                   float t,
                   std::array<std::array<float, noise_dim>, dim>& out) const
    {
    }
};
// el integrador hacer drift() * dt + diffusion() * dW
*/
/* modelo tiempo discreto generico: 
// puedes cambiar float por double, o incluos por int si tu estado siempre es entero
template<typename type = float>
struct Model {
    static constexpr size_t dim       = 1;
    static constexpr size_t noise_dim = 1;
    void drift(const std::array<type, dim>& x,
               int t,
               std::array<type, dim>& out) const
    {
    }
    void epsilon( const std::array<type, dim>& x,
               int t,
               std::array<std::array<type, noise_dim>, dim>& out) const
    {    
    }
    void diffusion(const std::array<type, dim>& x,
                   int t,
                   std::array<std::array<type, noise_dim>, dim>& out, 
                   std::array<type, dim>& out) const
    {              
    }
};
// el integrador hacer drift(x,t) + diffusion(x,t, epsilon(x,t))  
*/

namespace dsv {
namespace mod {

// --- Traits con valores por defecto (retrocompatibles) ---
template<typename M, typename = void>
struct model_state_type { using type = float; };
template<typename M>
struct model_state_type<M, std::void_t<typename M::state_type>> { using type = typename M::state_type; };

template<typename M, typename = void>
struct model_time_type { using type = float; };
template<typename M>
struct model_time_type<M, std::void_t<typename M::time_type>> { using type = typename M::time_type; };

template<typename M, typename = void>
struct is_discrete_model : std::false_type {};
template<typename M>
struct is_discrete_model<M, std::void_t<decltype(M::is_discrete)>>
    : std::bool_constant<M::is_discrete> {};

template<typename M>
inline constexpr bool is_discrete_v = is_discrete_model<M>::value;



// ModelRef: cero overhead para el caso local,
// un puntero raw para el caso compartido.
// std::variant<Model, Model*> ocupa sizeof(Model) + 1 byte.


template<typename Model>
struct ModelRef {
    std::variant<Model, Model*> storage { Model{} };

    Model& get() {
        return std::visit([](auto& v) -> Model& {
            if constexpr (std::is_pointer_v<std::decay_t<decltype(v)>>)
                return *v;
            else
                return v;
        }, storage);
    }
    const Model& get() const {
        return std::visit([](const auto& v) -> const Model& {
            if constexpr (std::is_pointer_v<std::decay_t<decltype(v)>>) return *v;
            else return v;
        }, storage);
    }


    void vincular(Model& externo)   { storage = &externo; }
    void usar_propio()              { storage = Model{};  }
    bool es_propio() const          { return std::holds_alternative<Model>(storage); }
};

// Instance genérica: reemplaza SIR_Instance, Lorenz_Instance, ORG_Instance, etc.
// --- Instance generalizada ---
template<typename Model>
struct Instance {
    using model_type = Model;
    using state_type = typename model_state_type<Model>::type;
    using time_type  = typename model_time_type<Model>::type;

    using state_t    = std::array<state_type, Model::dim>;

    ModelRef<Model> model_ref;
    state_t state {};
    time_type t {}; // 0.0f si es continuo, 0 si es discreto (int)

    Model&       getModel()       { return model_ref.get(); }
    const Model& getModel() const { return model_ref.get(); }
};



}  // end dsv
} // end mod
#endif // end hpp
