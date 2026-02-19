/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: DynSysVis - - - - - - - - - - - - - - - - - - - 
 */
/*  MAIN.cpp
    ejemplo para usar mi libreria 
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>


#include "DynSysVis.hpp"

using namespace dsv;
int main( ){
    // --- cargar colores --- DSV
    dsv::Color::cargar("assets/config/colores.txt");

    // --- configurar ventana --- SFML
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT - Colonia de Hormigas");

    // --- tablero con datos --- DSV
    dsv::Tablero tablero(window);


    // --- parametros para simular lo de hormigas ---
    float O = 60.0f;
    float G = 20.0f;
    float R = 10.0f;

    const float bO = 0.05f;
    const float betaOG = 4.0f;
    const float betaGR = 2.5f;
    const float betaRG = 1.2f;
    const float kG = 50.0f;
    const float kR = 50.0f;
    const float dG = 0.005f;
    const float dR = 0.005f;

   
    // / /  --- ---  PANELES --- ---  DSV  / / //

    /// GraficaTiempo
    auto guerreras    = tablero.add<dsv::GraficaTiempo>("guerreras", dsv::Color::rojo, dsv::Color::rojo);
    auto recolectoras = tablero.add<dsv::GraficaTiempo>("recolectoras", dsv::Color::oro, dsv::Color::oro);
    auto obreras      = tablero.add<dsv::GraficaTiempo>("obreras", dsv::Color::verde, dsv::Color::verde);

    guerreras.setPosition( dsv::Ubicacion::ArribaDer, 3,3);
    recolectoras.setPosition( dsv::RelativoA::Abajo, guerreras, 3,3);
    obreras.setPosition( dsv::RelativoA::Abajo, recolectoras, 3,3);

    /// GraficaEspacioFase
    auto OG = tablero.add<dsv::GraficaEspacioFase>("Espacio Fase (O, G)", dsv::Color::azul,  dsv::Color::azul );
    auto OR = tablero.add<dsv::GraficaEspacioFase>("Espacio Fase (O, R)", dsv::Color::aqua,  dsv::Color::aqua );
    auto RG = tablero.add<dsv::GraficaEspacioFase>("Espacio Fase (R, G)", dsv::Color::cian,  dsv::Color::cian );

    OG.setPosition( dsv::Ubicacion::ArribaIzq, 6,4 );
    OR.setPosition( dsv::RelativoA::Abajo, OG, 6,4 );
    RG.setPosition( dsv::RelativoA::Der, OR,  6,4 );

    /// GraficoCircular
    auto pie = tablero.add<dsv::GraficoCircular>("Poblacion de Hormigas", dsv::Color::aqua);

    pie.setPosition( dsv::RelativoA::Abajo, OR, 3, 2);

    pie -> personalizarColores( { dsv::Color::rojo, dsv::Color::oro, dsv::Color::verde } );


    /// GraficaTiempo // tambien admite multiserie
    auto triple = tablero.add<dsv::GraficaTiempo>("Poblaciones de Hormigas", dsv::Color::celeste );
    
    triple.setPosition( dsv::RelativoA::Izq, guerreras, 3,3);

    triple -> agregarSerie("guerreras"   , dsv::Color::rojo);
    triple -> agregarSerie("recolectoras", dsv::Color::oro);
    triple -> agregarSerie("obreras"     , dsv::Color::verde);


    // GraficaTiempo
    auto total = tablero.add<dsv::GraficaTiempo>("Poblaciones Total", dsv::Color::celeste );
    total.setPosition( dsv::RelativoA::Abajo, triple, 3,3);
    
        
    /// acceder a metodos especificos de los objetos de los paneles
    guerreras -> configurarMaxPoints(5000);
    recolectoras -> configurarMaxPoints(5000);
    obreras -> configurarMaxPoints(5000);
    triple -> configurarMaxPoints(5000);
    total -> configurarMaxPoints(5000);

    OG -> configurarMaxPoints(5000);
    OR -> configurarMaxPoints(5000);  
    RG -> configurarMaxPoints(5000);


    // --- --- --- ---  Control del tiempo --- --- --- --- 
    //  (Paso fijo de 0.1s)
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.005f); // Update por segundo
    // ups = sf::seconds(0.5f); // Update por segundo

    
   
    while( window.isOpen() ){

        // --- --- --- ---  eventos --- --- --- --- 
        sf::Event event;
        while( window.pollEvent(event) ){
            if (event.type == sf::Event::Closed) window.close();
        }

        // --- --- --- ---  Simulacion --- --- --- --- 
        accumulator += clock.restart();
        while( accumulator >= ups ){
            // --- Ec dif del MODELO NO LINEAL ---
            
            // dO/dt = bO*O - (betaOG / (kG + G))*O
            float dO = (bO * O) - (betaOG / (kG + G)) * O;   
            // dG/dt
            float dG_dt = (betaOG / (kG + G)) * O - (betaGR / (kR + R)) * G - (dG * G) + (betaRG / (kG + G)) * R;  
            // dR/dt
            float dR_dt = (betaGR / (kR + R)) * G - (betaRG / (kG + G)) * R - (dR * R);


            // actualiza variables con metodo de Euler
            float dt_val = ups.asSeconds();
            O += dO * dt_val;
            G += dG_dt * dt_val;
            R += dR_dt * dt_val;

           
            // agregar datos a graficas
            guerreras -> push_back(G);
            recolectoras -> push_back(R);
            obreras -> push_back(O);
            total -> push_back(O+G+R);

            triple -> push_back(O, "obreras");
            triple -> push_back(G, "guerreras");
            triple -> push_back(R, "recolectoras");

            OG -> push_back(O,G);
            OR -> push_back(O,R);
            RG -> push_back(R,G);

            pie -> push_back( {O, G, R} );


            accumulator -= ups;
        }

        // --- --- --- ---  Renderizado --- --- --- --- 
        tablero.draw();
        window.display();
    }

    return 0;
}