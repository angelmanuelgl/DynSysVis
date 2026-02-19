#include "Objeto.hpp" 
#include "Panel.hpp" 


namespace dsv {

// UNION DE GRAFICA Y OBJETO APRA EL USUARIO
template <typename T>
struct Vista {
    T& objeto;      // El objeto geneiroc como grafica tiempo, fase, circular
    Panel& panel;   // El contenedor

    // 1. Acceso directo a la gráfica
    T* operator->() { return &objeto; }

    // setPosicion UNFICIADA

    // Caso A: coordenadas
    void setPosition(float x, float y, double r=3, double c=4){
        panel.setSize(r,c);
        panel.setPosition(x, y);
    }

    // Caso B: Posicion Absoluta (Ubicación en la ventana)
    void setPosition(Ubicacion ubi, double r=3, double c=4){
        panel.setSize(r,c);
        panel.positionAbsoluta(ubi);
    }


    // Caso C: Posicion Relativa ( respecto a otro panel)
    void setPosition(RelativoA rel, const Panel& otro, double r=3, double c=4){
        panel.setSize(r,c);
        panel.positionRelativa(rel, otro);
    }

    // Caso C: Posciion Relatica (respecto a otra Vista)
    // Vista<U> para que sea fácil de usar entre diferentes graficas
    template <typename U>
    void setPosition(RelativoA rel, const Vista<U>& otra, double r=3, double c=4){
        panel.setSize(r,c);
        panel.positionRelativa(rel, otra.panel);
    }


};


} // termina dsv