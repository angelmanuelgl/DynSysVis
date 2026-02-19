/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/
/*  TITULO.hpp
    gestion de titulo
    para no sobrecagar el panel esto se hara aparte
    el panel se encargara de llamar a este objeto
*/
#ifndef TITULO_HPP
#define TITULO_HPP

namespace dsv{

class Titulo{
private:
    sf::Text elTitulo;
    float alturaReservada;
    // formato


public:
    Titulo(const std::string& str, const sf::Font& font, unsigned int size= 12){
        elTitulo.setFont(font);
        elTitulo.setString(str);
        elTitulo.setCharacterSize(size);
        elTitulo.setFillColor(sf::Color::White);
        // margen interno para que no pegue al borde del panel
        alturaReservada = elTitulo.getGlobalBounds().height + 15.f; 
    }

    void draw(sf::RenderWindow& window, sf::RenderStates states) {
        // ajuste fino de posicion dentro del panel
        elTitulo.setPosition(15.f, 10.f); 
        window.draw(elTitulo, states);
    }

    
    float getAltura() const { return alturaReservada; }

    // personalizar
    void setSize( unsigned int s ){
        elTitulo.setCharacterSize( s ); 
    }
    void setString(const std::string& s){ 
        elTitulo.setString(s); 
    }
    void setFont( const sf::Font& f ){ 
        elTitulo.setFont(f); 
    }
};


// end dvs
}
#endif