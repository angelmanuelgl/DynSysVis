/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/
/*  PANEL.hpp
    gestion de paneles dinamicos
    posicionamiento y obtienen mas


    el gestor del contenido, esto unifica
    -> objeto generico
    -> titulo
    -> posciion
*/


#include "Tablero.hpp"


namespace dsv {

    Tablero::Tablero( sf::RenderWindow& ventana,  sf::Color color1,  sf::Color color2)
        :window(ventana),colorFondo1(color1), colorFondo2(color2), fondo(sf::Quads, 4) {
            
        setFondoDegradado( colorFondo1, colorFondo2);
    }

    void Tablero::setFondoDegradado( sf::Color color1, sf::Color color2){
        sf::Vector2u size = window.getSize();

        // superior izquierda
        fondo[0].position = sf::Vector2f(0, 0);
        fondo[0].color = color1;

        // superior derecha
        fondo[1].position = sf::Vector2f(size.x, 0);
        fondo[1].color = color1;

        // inferior derecha
        fondo[2].position = sf::Vector2f(size.x, size.y);
        fondo[2].color = color2; 

        // inferior izquierda
        fondo[3].position = sf::Vector2f(0, size.y);
        fondo[3].color = color2;
    }

    void Tablero::draw() {
        // dibujar el fondo
        window.clear(colorFondo1);
        window.draw(fondo);

        // paneles
        for( auto& panel : paneles){
            panel->draw(); 
        }
    }



// termina dsv
}