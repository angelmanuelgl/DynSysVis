/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/

/*  GRAFICOCIRCULAR.hpp
    frafica circular (tipo pastel o dona)
    ->>. pie charts

*/

#ifndef GRAFICOCIRCULAR_HPP
#define GRAFICOCIRCULAR_HPP

// std y sfml
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>


// dsv 
#include "dsv/core/Objeto.hpp" // para que conozca el Objeto generico


namespace dsv{

class GraficoCircular : public Objeto{
protected:
    // circulo
    sf::VertexArray background;
    sf::VertexArray contorno;
    
    // el circulo 
    sf::Transform mytransform; 
    sf::Vector2f pos_actual;


    // divisiones
    int numSecciones;
    std::vector<float> valores;
    std::vector<sf::Color> colores;

    std::vector<sf::VertexArray> secciones;
    std::vector<sf::VertexArray> bordesExteriores;
    std::vector<sf::VertexArray> bordesInteriores;
    std::vector<sf::VertexArray> bordesRadiales;
    bool hayAlgunCambio = false;
    
    // estilo
    bool generarRellenoCirculo = false;
    float factorDona = 2.5f;
    
    //
    sf::Color colorBorde;
    float radio;
    float padding = 10.0f;

public:

    GraficoCircular(sf::Color bgColor = sf::Color(30,30,30) );
    void personalizarColores(const std::vector<sf::Color>& cols);

    /* --- FUNCIONES AUXILIARES --- */
    float aRadianes(float grados);
    sf::Vector2f puntoEnCirculo(sf::Vector2f centro, float radio, float grados );
    void limpiarGeometria();
    void reiniciar();
    void push_back(const std::vector<float>& vals);

    /* ---  GENERAR RELLENO CONTORNO --- */
    sf::VertexArray crearBordeRadial(sf::Vector2f centro, float rInt, float rExt, 
                                    float angInt, float angExt, sf::Color color );

    sf::VertexArray crearBorde( sf::Vector2f centro, float radio, float inicio, 
                                float barrido, sf::Color color );

    sf::VertexArray crearSector( sf::Vector2f centro, float radioExt, float inicioExt, float barridoExt, 
                                float inicioInt, float barridoInt,  sf::Color color);
   
    /* ---  GENERAR Y DIBUJAR --- */
    void generar(sf::Vector2f size );
    void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize );    


};

// end dvs
}

#endif 