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

    IMPORTANTE QUE SI CREAS TUN PROPIO COSA (grafo, poblacion, grafica, pie, etc) este HEREDE A objeto, 
    -> porque panel guarda    std::unique_ptr<Objeto> contenido;
*/

#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <SFML/Graphics.hpp>
#include "dsv/core/Logger.hpp"

namespace dsv{

// Objeto.hpp
class Objeto {
public:
    sf::Font font;

    // La fuente es esatica: compartida por todas las instancias
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
    


  

    // --- --- --- EVENTOS --- --- ---
    

    // actualizar (llamado externamente antes de gestionarEvento) reocmednaro llamar en el draw
    sf::FloatRect ultimoBoundsGlobal; 
    void actualizarBoundsGlobal(sf::RenderStates states, sf::Vector2f pSize) {
        sf::Vector2f posGlobal = states.transform.transformPoint(0, 0);
        this->ultimoBoundsGlobal = sf::FloatRect(posGlobal, pSize);
    }

    virtual void gestionarEvento(const sf::Event& ev, const sf::RenderWindow& win) {}


    // --- --- PARA CONTENIDO ESPECIAL --- --- 
    // no es necesario implementarla
    // solo la necesitaran los panelesFlotantes, en cualqueir caso estos tienen una medida por defecto asi que dont worry
    // solo es por si quieres ajustar tu mismo el size dependeinte del contenido, en los panelesFlotanes
    // los paneles "estaticos" tu defines el tamaño del panel y llama a su contenido para que se dibuje ajsutado a su tamaño
    virtual sf::Vector2f getSize() const {
        return sf::Vector2f(300.f, 600.f); 
    }
};


//  variables estaticas fuera de la clase // para que sean generales
// 'inline' (C++17)
inline sf::Font Objeto::fuenteCompartida;
inline bool Objeto::fuenteCargada = false;






}




#endif