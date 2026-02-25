/*

    EJEMPLO DE VISUALZIACION RAPDIA
    SIN PANELES SIN NADA
    PURA VISUALIZAICON
*/

#include "DynSysVis.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "DynSysVis - Test 3D");
    window.setFramerateLimit(60);

    dsv::Grafica3D plot;
    // Agregamos una serie con paleta Magma
    
    plot.getGestor().maxPointsGlobal = 100;
    plot.agregarSerie("lorenz", dsv::Color::Bosque());
    // Variables del Atractor de Lorenz
    float x = 0.1, y = 0, z = 0;
    float dt = 0.01;

    while( window.isOpen() ){
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            // la camra procesa el zoom con scrool y movimeinto con mouise
            plot.gestionarEvento(event, window);
        }

        // Lorenz
        float dx = 10.0 * (y - x);
        float dy = x * (28.0 - z) - y;
        float dz = x * y - (8.0 / 3.0) * z;
        x += dx * dt; y += dy * dt; z += dz * dt;

        // Mandamos a la serie (actualiza limites y buffer circular automáticamente)
        plot.push_back(x, y, z, "lorenz");

        window.clear(sf::Color(10, 10, 15)); // Fondo oscuro
        
        // Dibujamos la grafica (pSize es el tamaño del area de dibujo)
        plot.draw(window, sf::RenderStates::Default, sf::Vector2f(1200, 800));
        
        window.display();
    }
    return 0;
}