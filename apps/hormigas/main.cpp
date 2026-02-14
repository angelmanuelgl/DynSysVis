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

#include "Titulo.hpp"
#include "Libreria.hpp"
// #include "GraficoCircular.hpp"


int main( ){
    sf::Font roboto;
    roboto.loadFromFile("assets/fonts/Roboto.ttf");

    // --- cargar colores ---
    std::cout << "1. Cargando temas..." << std::endl;
    Tema::cargar("assets/config/colores.txt");

    // --- configurar ventana ---
    std::cout << "2. Inicializando ventana..." << std::endl;
    sf::RenderWindow window;
    Sistema::inicializarVentana(window, "Simulacion de Hormigas");


    // --- parametros para simular lo de hormigas ---

    // poblaciones iniciales
    float O = 30.0f; // Obreras
    float G = 30.0f;  // Guerreras
    float R = 40.0f;  // Recolectoras

    // transisciones y etc
    const float bO = 0.05f;     // natalidad obreras
    const float betaOG = 2.1f;   // transicion O->G
    const float betaGR = 0.9f;   // transicion G->R
    const float betaRG = 0.08f;  // transicion R->G
    const float kG = 80.0f;      // saturacion Guerreras
    const float kR = 80.0f;      // saturacion Recolectoras
    const float dG = 0.02f;      // muerte Guerreras
    const float dR = 0.04f;      // muerte Recolectoras

    std::cout << "3. Creando paneles..." << std::endl;
    // --- paneles derehca----
    Panel panelG(window, Tema::c("guerreras"), "Poblacion de Guerreras G(t)", 3,3 );
    panelG.positionAbsoluta(Ubicacion::ArribaDer);

    Panel panelR(window,  Tema::c("recolectoras"), "Poblacion de Recolectoras R(t)", 3,3  );
    panelR.positionRelativa(RelativoA::Abajo, panelG);

    Panel panelO(window,  Tema::c("obreras"), "Poblacion de Obreras O(t)", 3,3 );
    panelO.positionRelativa(RelativoA::Abajo, panelR);
    
     // --- paneles izquieda----
    Panel panelfOG(window, Tema::c("color1"), "Espacio Fase (O, G)", 6, 4);
    panelfOG.positionAbsoluta(Ubicacion::ArribaIzq);

    Panel panelfOR(window,  Tema::c("color2"), "Espacio Fase (O, R)", 6, 4);
    panelfOR.positionRelativa(RelativoA::Abajo  , panelfOG);

    Panel panelfRG(window,  Tema::c("color3"), "Espacio Fase (R, G)", 6, 4);
    panelfRG.positionRelativa(RelativoA::Der  , panelfOR);

    Panel panelCirc(window,  Tema::c("rojo"), "Poblacion de Hormigas", 3, 2);
    panelCirc.positionRelativa(RelativoA::Abajo, panelfOR);

    // --- paneles centro----
    Panel panelTriple(window, Tema::c("celeste"),"Poblacion de Hormigas", 3, 3); 
    panelTriple.positionRelativa(RelativoA::Izq  , panelG);
    
    
    // --- graficas respecto a tiempo y respecot a fase ---
    auto* graphG = panelG.crearContenido<GraficaTiempo>(Tema::c("guerreras"));
    auto* graphR = panelR.crearContenido<GraficaTiempo>(Tema::c("recolectoras"));
    auto* graphO = panelO.crearContenido<GraficaTiempo>(Tema::c("obreras"));

    auto* graphTriple = panelTriple.crearContenido<GraficaTiempo>(Tema::c("guerreras"));
    graphTriple -> agregarSerie("guerreras",Tema::c("guerreras"));
    graphTriple -> agregarSerie("recolectoras",Tema::c("recolectoras"));
    graphTriple -> agregarSerie("obreras",Tema::c("obreras"));
    
    graphTriple -> configurarMaxPoints(5000);
    graphG -> configurarMaxPoints(5000);
    graphR -> configurarMaxPoints(5000);
    graphO -> configurarMaxPoints(5000);

    auto* faseOG = panelfOG.crearContenido<GraficaEspacioFase>(Tema::c("color1"));
    auto* faseOR = panelfOR.crearContenido<GraficaEspacioFase>(Tema::c("color2"));
    auto* faseRG = panelfRG.crearContenido<GraficaEspacioFase>(Tema::c("color3"));

    faseOG -> configurarMaxPoints(10000);
    faseOR -> configurarMaxPoints(10000);  
    faseRG -> configurarMaxPoints(10000);


    std::vector<sf::Color> colores = { Tema::c("obreras"), Tema::c("guerreras"), Tema::c("recolectoras") };
    auto* circular = panelCirc.crearContenido<GraficoCircular>();
    circular -> personalizarColores( colores );



    // --- IMPORTANTE: control del tiempo --
    //  (Paso fijo de 0.1s)
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.008f); // Update por segundo
    // ups = sf::seconds(0.5f); // Update por segundo

    std::cout << "5. Inicializando simulacion   ..." << std::endl;
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
            graphO -> addValue(O);
            graphG -> addValue(G);
            graphR -> addValue(R);

            graphTriple -> addValue(O, "obreras");
            graphTriple -> addValue(G, "guerreras");
            graphTriple -> addValue(R, "recolectoras");

            faseOG -> addValue(O,G);
            faseOR -> addValue(O,R);
            faseRG -> addValue(R,G);

            circular -> addValues( {O, G, R} );


            accumulator -= ups;
        }

        // --- RENDERIZADO ---
        window.clear(sf::Color(40, 40, 40)); // fondo oscuro tipo Sci-Fi

        // Dibujar paneles y sus graficas internas
        panelG.draw();
        panelR.draw();
        panelO.draw();
        panelfOG.draw();
        panelfOR.draw();
        panelfRG.draw();

        panelCirc.draw();

        panelTriple.draw();

        window.display();
    }

    return 0;
}