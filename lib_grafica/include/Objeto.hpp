#ifndef OBJETO_HPP
#define OBJETO_HPP

#include <SFML/Graphics.hpp>

class Objeto {
public:
    // destructor virtual 
    virtual ~Objeto() = default;

    // Todas las coasas (graficas eje xy (espacio fase, grafica de tiempo), grafioco circulares, dibuajr grafo etce tc)
    //  heredaran este metodo
     // ojo: quen no conviene que reciva el panel
    virtual void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize ) = 0;

};

#endif