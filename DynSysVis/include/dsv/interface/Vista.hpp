/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/
/*  Vista.hpp
    Contiene la union del objeto generico que peude ser grafica o cualquier cosa 
    que herede de objeto
*/

#ifndef VISTA_HPP
#define VISTA_HPP

#include "dsv/core/Objeto.hpp"
#include "dsv/core/Panel.hpp"

namespace dsv {


// UNION DE GRAFICA Y OBJETO APRA EL USUARIO
template <typename T>
struct Vista {
    T& objeto;      // el objeto generico como grafica tiempo, fase, circular
    Panel& panel;   // el contenedor

    //  Acceso directo a metodos de la grafica(porejemplo) 
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
#endif // termina VISTA_HPP