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

    // dsv::Layout miLayout = {
    //     "3d 3d 3d f2 f2",
    //     "3d 3d 3d f2 f2",
    //     "3d 3d 3d f3 f4  ",
    // };
    dsv::Layout miLayout = {
        "3d 3d  f2",
        "3d 3d  f3 ",
        "3d 3d  f4   ",
    };
    dsv::Tablero tablero(window, miLayout,  sf::Color(60, 60, 60),  sf::Color(40, 40, 40));

    // Paneles
    auto faseXY = tablero.add<dsv::EspacioFase2D>("Fase: XY", dsv::Color::azul, "f2");
    auto faseYZ = tablero.add<dsv::EspacioFase2D>("Fase: YZ", dsv::Color::azul, "f3");
    auto faseXZ = tablero.add<dsv::EspacioFase2D>("Fase: XZ", dsv::Color::azul, "f4");

    auto fase3D = tablero.add<dsv::Grafica3D>("Trayectorias Modelo Lorentz", dsv::Color::cian, "3d");
    auto tiempo = tablero.add<dsv::GraficaTiempo>("Evolución Infectados (20 Tiradas)", dsv::Color::naranja, "t1");

    // Configuración visual
    tiempo->configurarLimites(0, 50, 0, 100, true);
    tiempo->ponerSombreado(false);

    for( auto fff : { faseXY, faseYZ, faseXZ } ){
        fff->activarSeguimiento(true);
        fff->configurarMaxPoints(1000);
    }
    
    // Inicializar 20 instancias
    const int numSims = 40;
    std::vector<dsv::mod::Lorenz_Instance> sims(numSims);
    
    // rangos aleatorios
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<float> uniform( -20.0f, 20.0f );



    for( int i = 0; i < numSims; i++ ){
        std::string id = "Sim" + std::to_string(i);
        sf::Color col = dsv::Color::Magma(i, numSims);
        col.a = 100;

    
        sims[i].state = { uniform(gen),uniform(gen),uniform(gen)}; 
        
        
        for( auto fff : { faseXY, faseYZ, faseXZ} )
            fff-> agregarSerie(id, col);

        tiempo->agregarSerie(id, col);

        col.a = 70;
        fase3D->agregarSerie(id, col);
    }

    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.01f);
    bool iniciado = false;

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::KeyPressed){
                iniciado = true;
                accumulator = sf::Time::Zero;
                clock.restart();
            } 
            fase3D->gestionarEvento(event, window);
        }

        accumulator += clock.restart();
        while(accumulator >= ups && iniciado) {

            float dt=  ups.asSeconds()/10 ; // velocidad x0.5
            // actualizar cada  instancia            
            for( int i = 0; i < numSims; i++){
                auto& s = sims[i];
                std::string id = "Sim" + std::to_string(i);

                //  integrador
                dsv::sim::step(s, dt);

                // Llenar graficas
                faseXY->push_back(s.state[0], s.state[1], id);
                faseYZ->push_back(s.state[1], s.state[2], id);
                faseXZ->push_back(s.state[0], s.state[2], id);

                tiempo->push_back(s.state[1], s.t, id);
                fase3D->push_back(s.state[0], s.state[1], s.state[2], id);
            }
            accumulator -= ups;
        }

     
            tablero.draw();
            window.display();
        
     
    }
    return 0;
}