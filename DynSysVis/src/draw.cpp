/*
    Utilidades de dibujo // asi como Graphics2D/Geometria
    pero enfocado a ser totoalmente generla

*/

#include "dsv/geodraw/draw.hpp"


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
                                    bool grosorDinamico) {
    size_t n = puntos.size();
    va.setPrimitiveType(sf::TriangleStrip);
    va.resize(n * 2);

    for( size_t i = 0; i < n; ++i ){
        // direccion perpendicular
        sf::Vector2f dir;
        if (i < n - 1) dir = puntos[i + 1] - puntos[i];
        else dir = puntos[i] - puntos[i - 1];

        // degradado
        float t = static_cast<float>(i) / (n - 1);
        sf::Color col = Color::obtenerColorDegradado(paleta, t);
        
        // grosor dinamico
        float grosorVariable = grosor;
        if( grosorDinamico )
            grosorVariable = grosor * t;

        // normalizacion y vector perpendiucla
        float mag = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        sf::Vector2f normal = (mag > 0.001f) ? sf::Vector2f(-dir.y / mag, dir.x / mag) : sf::Vector2f(0.f, 0.f);
        sf::Vector2f offset = normal * (grosorVariable / 2.0f);

       

        // unir con TriangleStrip
        va[2 * i + 0].position = puntos[i] + offset;
        va[2 * i + 1].position = puntos[i] - offset;
        va[2 * i + 0].color = col;
        va[2 * i + 1].color = col;
    }
}

// OPCION A .. Vector de colores (para degradados)
void linea(sf::RenderWindow& window, sf::RenderStates states,
                    const std::vector<sf::Vector2f>& puntos, 
                    const std::vector<sf::Color>& colores, 
                    float grosor, 
                    bool grosorDinamico) {
      if( puntos.size() < 2) return;
    sf::VertexArray va;
    generarGeometriaLinea(va, puntos, colores, grosor, grosorDinamico);
    window.draw(va, states);
}

// OPCION B // Color unico (Para ejes o series solidas)
void linea(sf::RenderWindow& window, sf::RenderStates states,
                    const std::vector<sf::Vector2f>& puntos, 
                    sf::Color color, 
                    float grosor, 
                    bool grosorDinamico) {
    if( puntos.size() < 2) return;
    std::vector<sf::Color> colores = {color};
    sf::VertexArray va;
    generarGeometriaLinea(va, puntos, colores, grosor, grosorDinamico);
    window.draw(va, states);
}


} // end draw
} // end dsv
