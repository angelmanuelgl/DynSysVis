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
    APLICACION: sistema dinamicos de hormigas
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

    unsigned int ancho = static_cast<unsigned int>(desktop.width * 0.8f);
    unsigned int alto = static_cast<unsigned int>(desktop.height * 0.8f);
    sf::RenderWindow window(sf::VideoMode(ancho, alto), 
                        "Simulacion de Colonia de Hormigas - Tesis", 
                        sf::Style::Fullscreen , //  Default
                        settings);
    // window.setPosition(sf::Vector2i(
    //     (desktop.width - ancho) / 2,
    //     (desktop.height - alto) / 2
    // ));

    window.setFramerateLimit(60);

    // --- Parametros del Pendulo ---
    float theta = 1.5f;      // angulo inicial (aprox 85 grados)
    float omega = 0.0f;      // velocidad angular inicial
    const float g = 9.81f;   // gravedad
    const float L = 200.0f;  // longitud del pendulo (en pixeles para visualización)
    const float amortiguamiento = 3.995f; // Opcional: para que se detenga poco a poco

    // --- Interfaz InsightRT ---
    float espaciado = 25.f;
    float margenVentana = 20.f;

    // Panel para el angulo Theta(t)
    Panel panelTheta({400, 250}, 20, sf::Color(25, 25, 25), sf::Color::Yellow);
    panelTheta.positionAbsoluta(Ubicacion::CentroDer, window, margenVentana);
    GraficaTiempo graphTheta(100, sf::Color::Yellow, "Angulo Theta(t)");

    // Panel para velocidad angular omega(t)
    Panel panelOmega({400, 250}, 20, sf::Color(25, 25, 25), sf::Color::Yellow);
    panelOmega.positionRelativa(RelativoA::Arriba, panelTheta, espaciado);
    GraficaTiempo graphOmega(100, sf::Color::Yellow, "Velocidad Omega(t)");

    // panel para el Espacio de Fase (Theta vs Omega)
    Panel panelFase({400, 250}, 20, sf::Color(25, 25, 25), sf::Color::Magenta);
    panelFase.positionRelativa(RelativoA::Abajo, panelTheta, espaciado);
    GraficaEspacioFase graphFase(100, sf::Color::Magenta, "Fase (Theta, Omega)");

    // --- Control del tiempo ---
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.016f); // ~60 updates por segundo para suavidad
    // sf::Time ups = sf::seconds(0.1f); // Update por segundo

    while( window.isOpen() ){
        sf::Event event;
        while( window.pollEvent(event) ){
            if (event.type == sf::Event::Closed) window.close();
        }

        accumulator += clock.restart();
        while( accumulator >= ups ){
            float dt = ups.asSeconds();

            // --- pendulo (Euler-Cromer para mejor estabilidad) ---
            float aceleracion = -(g / (L / 100.0f)) * std::sin(theta); // L/100 para escalar metros
            omega += aceleracion * dt;
            // omega *= amortiguamiento; // Descomenta para friccion
            theta += omega * dt;

            // actualizar graficas
            graphTheta.addValue(theta);
            graphOmega.addValue(omega);
            graphFase.addValue(theta, omega);

            accumulator -= ups;
        }

        // --- RENDERIZADO ---
        window.clear(sf::Color(10, 10, 10));

        // dibujar el pendulo fisicamente
        sf::Vector2f origen(window.getSize().x / 4.0f, window.getSize().y / 2.0f);
        sf::Vector2f extremo(origen.x + L * std::sin(theta), origen.y + L * std::cos(theta));

        sf::Vertex linea[] = { sf::Vertex(origen), sf::Vertex(extremo, sf::Color::White) };
        window.draw(linea, 2, sf::Lines);

        sf::CircleShape masa(15.f);
        masa.setFillColor(sf::Color::Cyan);
        masa.setOrigin(15.f, 15.f);
        masa.setPosition(extremo);
        window.draw(masa);

        // paneles y graficas
        panelTheta.draw(window);
        graphTheta.draw(window, panelTheta);

        panelFase.draw(window);
        graphFase.draw(window, panelFase);
        
        panelOmega.draw(window);
        graphOmega.draw(window, panelOmega);


        window.display();
    }
    return 0;
}