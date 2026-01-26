/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: InsightRT - - - - - - - - - - - - - - - - - - - 
    * libreria de herramientas graficas para monitoreo de datos 
    * en vivo y comportamiento de sistemas complejos.
 */
/*  MAIN.cpp
    ejemplo para usar mi libreria InsightRT
    sistema dinamicos de hormigas
*/
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Geometria.hpp"
#include "Graficas.hpp"



int main( ){
    // --- configurar ventana ---
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop,  "Simulacion de Colonia de Hormigas - Tesis",  sf::Style::Fullscreen, settings);

    window.setFramerateLimit(60);

    // --- poaraemtros apa simualr lko de hormigas ---

    // poblaciones iniciales
    float O = 100.0f; // Obreras
    float G = 20.0f;  // Guerreras
    float R = 10.0f;  // Recolectoras

    // transisciones y etc
    const float bO = 0.102f;     // natalidad obreras
    const float betaOG = 0.1f;   // transición O->G
    const float betaGR = 0.1f;   // transición G->R
    const float betaRG = 0.08f;  // transición R->G
    const float kG = 50.0f;      // saturacion Guerreras
    const float kR = 40.0f;      // saturacion Recolectoras
    const float dG = 0.02f;      // muerte Guerreras
    const float dR = 0.05f;      // muerte Recolectoras

    // ---  INterfaz ---
    float espaciado = 25.f; // epacio entre paneles
    float margenVentana = 20.f;

    // --- Guerreras G(t) ----
    Panel panelG({350, 200}, 20, sf::Color(30, 60, 30), sf::Color(0, 255, 255));
    panelG.positionAbsoluta(Ubicacion::CentroDer, window, margenVentana);
    GraficaTiempo graphG(100, sf::Color::Cyan, "Poblacion de Guerreras G(t)");

    // --- Recolectoras R(t) ---
    Panel panelR({350, 200}, 20, sf::Color(30, 30, 30), sf::Color(255, 100, 0));
    panelR.positionRelativa(RelativoA::Arriba, panelG, espaciado);
    GraficaTiempo graphR(100, sf::Color(255, 100, 0), "Poblacion de Recolectoras R(t)");

    // --- Obreras O(t) ---
    Panel panelO({350, 200}, 20, sf::Color(30, 30, 30), sf::Color(100, 0, 255));
    panelO.positionRelativa(RelativoA::Abajo, panelG, espaciado);
    GraficaTiempo graphO(100, sf::Color(100, 0, 255), "Poblacion de Obreras O(t)");
    graphO.ponerSobreado(false);

    // --- boceto de fase ---
    Panel nuevoPanel({350, 200}, 20, sf::Color(30, 30, 30), sf::Color(100, 0, 255));
    nuevoPanel.positionAbsoluta(Ubicacion::CentroIzq, window, margenVentana);
    GraficaEspacioFase nuevaGrafica(100, sf::Color(100, 0, 255), "(Obreras, Guerreras)");

    // --- IMPORTANTE: control del tiempo --
    //  (Paso fijo de 0.1s)
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.1f); // Update por segundo

    while( window.isOpen() ){
        sf::Event event;
        while( window.pollEvent(event) ){
            if (event.type == sf::Event::Closed) window.close();
        }

        // --- simulacion ---
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
            graphG.addValue(G);
            graphR.addValue(R);
            graphO.addValue(O);
            nuevaGrafica.addValue(O,G);

            accumulator -= ups;
        }

        // --- RENDERIZADO ---
        window.clear(sf::Color(15, 15, 15)); // Fondo oscuro tipo Sci-Fi

        // Dibujar paneles y sus gráficas internas
        panelG.draw(window);
        graphG.draw(window, panelG);

        panelR.draw(window);
        graphR.draw(window, panelR);

        panelO.draw(window);
        graphO.draw(window, panelO);

        nuevoPanel.draw(window);
        nuevaGrafica.draw(window, nuevoPanel);

        

        window.display();
    }

    return 0;
}