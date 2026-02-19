#include <SFML/Graphics.hpp>
#include <cmath>
#include "DynSysVis.hpp"


int main(){
    // --- cargar colores --- DSV
    dsv::Color::cargar("assets/config/colores.txt");

    // --- configurar ventana --- SFML
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT - TITULO");

    // --- tablero con datos --- DSV
    dsv::Tablero tablero(window);


    // --- parametros --- 
    
    
    // --- paneles ---
    // "Titulo", color del panel, parametros del contenido
    auto panel_a = tablero.add<dsv::GraficaTiempo>("Tiempo", dsv::Color::rojo, dsv::Color::rojo);
    auto panel_b = tablero.add<dsv::GraficaEspacioFase>("Fase", dsv::Color::rojo, dsv::Color::rojo);
    auto panel_c = tablero.add<dsv::GraficoCircular>("Pie", dsv::Color::rojo);
    
    // setPosition // admite ubicacion relativa y absoluta 
    // Ubicacion  (opcioneles:) filas columnas
    // RelativoA, otroPanel  (opcioneles:) filas columnas
    panel_a.setPosition( dsv::Ubicacion::ArribaDer, 2,4 );
    panel_b.setPosition( dsv::RelativoA::Abajo, panel_a, 2,4);
    panel_c.setPosition( dsv::RelativoA::Abajo, panel_b );


    

}