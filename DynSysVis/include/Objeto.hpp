/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/
/*
    Objeto.hpp
    objeto generico que del cual heredar para ponerlod entro de panael facilmente
    tu solo implementa la funcion:
    
    virtual void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize) = 0;
    
    el panel se encargara del resto, llamr tu funcion, colocarlo dentro
*/

#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <SFML/Graphics.hpp>
#include <Logger.hpp>

namespace dsv{

// Objeto.hpp
class Objeto {
public:
    sf::Font font;

    // La fuente es estática: compartida por todas las instancias
    static sf::Font fuenteCompartida;
    static bool fuenteCargada;

    // buscar fuente
    static void cargarFuenteGlobal() {
        if( fuenteCargada ) return; // Si ya se cargo, no hacer nada

        std::vector<std::string> rutas = {
            "../../assets/fonts/Roboto.ttf",
            "../assets/fonts/Roboto.ttf",
            "assets/fonts/Roboto.ttf"
        };

        for( const auto& ruta : rutas ){
            if( fuenteCompartida.loadFromFile(ruta) ){
                DSV_LOG_SUCCESS("Fuente cargada globalmente desde: " + ruta);
                fuenteCargada = true;
                return;
            }
        }
        DSV_LOG_SUCCESS("NO se encontro Roboto.ttf en ninguna ruta");
    }

    Objeto() {
        // si no esta cargada se carga
        cargarFuenteGlobal();
    }

    virtual ~Objeto() = default;
    virtual void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize) = 0;
};


//  variables estaticas fuera de la clase // para que sean generales
// 'inline' (C++17)
inline sf::Font Objeto::fuenteCompartida;
inline bool Objeto::fuenteCargada = false;






}




#endif