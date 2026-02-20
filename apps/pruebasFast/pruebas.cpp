#include "dynsysvis.hpp"
#include <iostream>
#include <vector>

using namespace std;
int main(){
    // --- cargar colores --- DSV
    dsv::Color::cargar("assets/config/colores.txt");

    // --- configurar ventana --- SFML
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT - Colonia de Hormigas");

    // --- tablero con datos --- DSV
    dsv::Layout miLayout = {
        "A ",
    };
    dsv::Tablero tablero(window, miLayout);

    auto grafica = tablero.add<dsv::Grafica3D>("Titul", dsv::Color::rojo, "A");


    
    // --- --- --- ---  simulacion --- --- --- --- 
    float t=0;
    float x = 0.1, y = 0.0, z = 0.0;
    float sigma = 10.0, rho = 28.0, beta = 8.0/3.0;

    // --- --- --- ---  Control del tiempo --- --- --- --- 
    //  (Paso fijo de 0.1s)
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.005f); // Update por segundo
    bool iniciado = false;
    // ups = sf::seconds(0.5f); // Update por segundo

   int colorido = 0;
    while( window.isOpen() ){
        // --- --- --- ---  eventos --- --- --- --- 
        sf::Event event;
        while( window.pollEvent(event) ){
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed){
                clock.restart(); 
                accumulator = sf::Time::Zero;
                iniciado = true;
            }

            grafica->gestionarEvento(event, window);
        }

        // --- --- --- ---  Simulacion --- --- --- --- 
        accumulator += clock.restart();
        
        while( accumulator >= ups && iniciado ){
            float dt = ups.asSeconds();
            t +=  dt;

            if( t < 5 ){
                grafica->push_back(t*10,0,0, "hi", dsv::Color::blanco);
                grafica->push_back(0,t*10,0, "hii", dsv::Color::blanco);
                grafica->push_back(0,0,t*10, "hiii", dsv::Color::blanco);
            }
          


            // Dentro del while de simulacion:
            float dx = sigma * (y - x) * dt;
            float dy = (x * (rho - z) - y) * dt;
            float dz = (x * y - beta * z) * dt;
            x += dx; y += dy; z += dz;

     

            accumulator -= ups;
        }
        // --- --- --- ---  Actualizar --- --- --- --- 
        colorido++; if( colorido> 2000 ) colorido = 0;
        grafica->push_back(x, y, z, "Lorenz", dsv::Color::FuegoHielo( colorido, 2000 ));

        // --- --- --- ---  Renderizado --- --- --- --- 
        tablero.draw();
        window.display();


    }


    return 0;
}