/*
    Utilidades de dibujo // asi como Graphics2D/Geometria
    pero enfocado a ser totoalmente generla

*/

#ifndef DRAW_HPP
#define DRAW_HPP

//  sfml y std
#include <SFML/Graphics.hpp>
#include <cmath>

#include "dsv/interface/Temas.hpp"

namespace dsv{
namespace draw{


/*
    --- --- --- --- -- --- --- --- --- 
    --- --- ---  L I N E A --- --- --- 
    --- --- --- --- -- --- --- --- --- 
*/


// estructura para no repetir calculos
void generarGeometriaLinea(sf::VertexArray& va, 
                                    const std::vector<sf::Vector2f>& puntos, 
                                    const std::vector<sf::Color>& paleta, 
                                    float grosor, 
                                    bool grosorDinamico = false);
                                    
// OPCION A // Vector de colores (para degradados)
void linea(sf::RenderWindow& window, sf::RenderStates states,
                    const std::vector<sf::Vector2f>& puntos, 
                    const std::vector<sf::Color>& colores, 
                    float grosor, 
                    bool grosorDinamico = false);

// OPCION B // Color unico (Para ejes o series solidas)
void linea(sf::RenderWindow& window, sf::RenderStates states,
                    const std::vector<sf::Vector2f>& puntos, 
                    sf::Color color, 
                    float grosor, 
                    bool grosorDinamico = false);




} // end draw
} // end dsv
#endif//en inlclude


