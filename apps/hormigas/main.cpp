/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: InsightRT - - - - - - - - - - - - - - - - - - - 
    * libreria de herramientas graficas para monitoreo de datos 
    * en en tiempo real y comportamiento de sistemas complejos.
 */
/*  MAIN.cpp
    ejemplo para usar mi libreria InsightRT
    sistema dinamicos de hormigas
*/
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Libreria.hpp"



int main( ){
    // --- cargar colores ---
    Tema::cargar("assets/config/colores.txt");

    // --- configurar ventana ---
    sf::RenderWindow window;
    Sistema::inicializarVentana(window, "Simulación de Hormigas - Tesis");


    // --- parametros para simular lo de hormigas ---

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


    // --- paneles----
    Panel panelG(window, Tema::c("guerreras"), 4,3 );
    panelG.positionAbsoluta(Ubicacion::ArribaDer);

    Panel panelR(window,  Tema::c("recolectoras"),  4,3 );
    panelR.positionRelativa(RelativoA::Abajo, panelG);

    Panel panelO(window,  Tema::c("obreras"), 4,3 );
    panelO.positionRelativa(RelativoA::Abajo, panelR);

    Panel panelf1(window, Tema::c("color1"), 3, 2);
    panelf1.positionAbsoluta(Ubicacion::ArribaIzq);

    Panel panelf2(window,  Tema::c("color2"), 3, 2);
    panelf2.positionRelativa(RelativoA::Abajo  , panelf1);


    // --- graficas respecto a tiempo y respecot a fase ---
    auto* graphG = panelG.crearContenido<GraficaTiempo>(Tema::c("guerreras"), "Poblacion de Guerreras G(t)");
 
    auto* graphR = panelR.crearContenido<GraficaTiempo>(Tema::c("recolectoras"), "Poblacion de Recolectoras R(t)");
   
    auto* graphO = panelO.crearContenido<GraficaTiempo>(Tema::c("obreras"), "Poblacion de Obreras O(t)");
   
    auto* fase1 = panelf1.crearContenido<GraficaEspacioFase>(Tema::c("color1"), "(Obreras, Guerreras)");
  
    auto* fase2 = panelf2.crearContenido<GraficaEspacioFase>(Tema::c("color2"), "(Obreras, Recolectoras)");


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
            graphG -> addValue(G);
            graphR -> addValue(R);
            graphO -> addValue(O);
            fase1 -> addValue(O,G);
            fase2 -> addValue(O,R);

            accumulator -= ups;
        }

        // --- RENDERIZADO ---
        window.clear(sf::Color(15, 15, 15)); // Fondo oscuro tipo Sci-Fi

        // Dibujar paneles y sus graficas internas
        panelG.draw();
        panelR.draw();
        panelO.draw();
        panelf2.draw();
        panelf1.draw();

        window.display();
    }

    return 0;
}