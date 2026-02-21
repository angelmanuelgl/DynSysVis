#include <SFML/Graphics.hpp>
#include <cmath>
#include "DynSysVis.hpp"

struct EntidadSIR {
    dsv::mod::SIR_Instance sim; // Los datos puros de simulación
    std::string id;             // Metadato visual
    sf::Color color;            // Metadato visual
};

int main() {
    dsv::Color::cargar("assets/config/colores.txt");
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT - 20 Simulaciones SIR");

    dsv::Layout miLayout = {
        "f1 f1 f2 f2",
        "f1 f1 f2 f2",
        "t1 t1 t1 t1"
    };
    dsv::Tablero tablero(window, miLayout,  sf::Color(60, 60, 60),  sf::Color(40, 40, 40));

    // Paneles
    auto fase = tablero.add<dsv::EspacioFase2D>("Fase: Susceptibles vs Infectados", dsv::Color::azul, "f1");
    auto fase3D = tablero.add<dsv::Grafica3D>("Trayectorias SIR (S,I,R)", dsv::Color::cian, "f2");
    auto tiempo = tablero.add<dsv::GraficaTiempo>("Evolución Infectados (20 Tiradas)", dsv::Color::naranja, "t1");

    // Configuración visual
    fase->configurarLimites(0, 100, 0, 100, true);
    tiempo->configurarLimites(0, 50, 0, 100, true);
    tiempo->ponerSombreado(false);
    
    // Inicializar 20 instancias
    const int numSims = 20;
    std::vector<dsv::mod::SIR_Instance> sims(numSims);
    

    for( int i = 0; i < numSims; i++ ){
        std::string id = "Sim" + std::to_string(i);
        sf::Color col = dsv::Color::Oceano(i, numSims);
        sims[i].state = {200.0f, 10.0f, 0.0f}; // S=99, I=1, R=0
        
        fase->agregarSerie(id, col);
        tiempo->agregarSerie(id, col);
        fase3D->agregarSerie(id, col);
    }

    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.066f);
    bool iniciado = false;

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::KeyPressed) iniciado = true;
            fase3D->gestionarEvento(event, window);
        }

        accumulator += clock.restart();
        while(accumulator >= ups && iniciado) {
            // actualizar cada  instancia            
            for( int i = 0; i < numSims; i++){
                auto& s = sims[i];
                std::string id = "Sim" + std::to_string(i);

                //  integrador
                dsv::sim::step(s, ups.asSeconds());

                // Llenar graficas
                fase->push_back(s.state[0], s.state[1], id);
                tiempo->push_back(s.state[1], s.t, id);
                fase3D->push_back(s.state[0], s.state[1], s.state[2], id);
            }
            accumulator -= ups;
        }
        if( iniciado ){
            tablero.draw();
            window.display();
        }
     
    }
    return 0;
}