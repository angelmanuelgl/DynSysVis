// ** BOCETO RAPIDO ** POSIBLEMENE BORRARE EST E ARHCIVOS

#ifndef GRAFICA3D_HPP
#define GRAFICA3D_HPP

//  sfml y std
#include <SFML/Graphics.hpp>
#include <cmath>

#include "dsv/core/Objeto.hpp"
#include "dsv/dataStructures/Estructuras.hpp"


namespace dsv {

class Grafica3D : public Objeto {
protected:
    // Almacenamos series que usan la cola 3D que creamos
    std::map<std::string, MonotonicQueue3D<float>> datosSeries;
    std::map<std::string, sf::Color> coloresSeries;

    Limites3D lim;
    
    // Parámetros de la cámara y visualización
    float rotacionX = 45.0f;
    float rotacionY = 45.0f;
    float zoom = 1.0f;

    sf::Vector2i ultimaPosMouse;
    bool estaRotando = false;
    float sensibilidad = 0.4f;

public:
    Grafica3D(){
        lim = {-1, 1, -1, 1, -1, 1};

        // agregar ejes // todo mas control, ahcelo opcional, gestioanr limites
        push_backC(0,0,0, "X", sf::Color::Red);
        push_backC(50,0,0, "X", sf::Color::Red);
        push_backC(0,0,0, "Y", sf::Color::Blue);
        push_backC(0,50,0, "Y", sf::Color::Blue);
        push_backC(0,0,0, "Z", sf::Color::Green);
        push_backC(0,0,50, "Z", sf::Color::Green);
    }

    void agregarSerie(std::string nombre, sf::Color color){
         coloresSeries[nombre] = color;
    }


    // Agregar puntos a una serie especifica
    void push_backC(float x, float y, float z, std::string id = "default", sf::Color color = sf::Color::Cyan){
        datosSeries[id].push(x, y, z);
        coloresSeries[id] = color;
        actualizarLimites(x, y, z);
    }
    void push_back(float x, float y, float z, std::string id = "default"){
        datosSeries[id].push(x, y, z);
        if( coloresSeries.find(id) == coloresSeries.end() )  coloresSeries[id] = sf::Color::Cyan;
        actualizarLimites(x, y, z);
    }

    void actualizarLimites(float x, float y, float z) {
        if (x < lim.minX) lim.minX = x; 
        if (x > lim.maxX) lim.maxX = x;

        if (y < lim.minY) lim.minY = y; 
        if (y > lim.maxY) lim.maxY = y;

        if (z < lim.minZ) lim.minZ = z;
        if (z > lim.maxZ) lim.maxZ = z;

        // ---  OJO CON DIVIDIR ENTRE 0 --- 
        float epsilon = 0.001f;
        if( abs(lim.maxX - lim.minX) < epsilon ){
            lim.minX -= 1.0f;
            lim.maxX += 1.0f;
        }
        if( std::abs(lim.maxY - lim.minY) < epsilon ){
            lim.minY -= 1.0f;
            lim.maxY += 1.0f;
        }
         if( std::abs(lim.maxZ - lim.minZ) < epsilon ){
            lim.minZ -= 1.0f;
            lim.maxZ += 1.0f;
        }
    }

    // Función mágica: Proyecta 3D -> 2D
    sf::Vector2f proyectar(sf::Vector3f p, sf::Vector2f pSize) {
        // 1. Normalizar el punto respecto a los límites (rango -1 a 1)
        float nx = (lim.maxX != lim.minX) ? 2.0f * (p.x - lim.minX) / (lim.maxX - lim.minX) - 1.0f : 0;
        float ny = (lim.maxY != lim.minY) ? 2.0f * (p.y - lim.minY) / (lim.maxY - lim.minY) - 1.0f : 0;
        float nz = (lim.maxZ != lim.minZ) ? 2.0f * (p.z - lim.minZ) / (lim.maxZ - lim.minZ) - 1.0f : 0;

        // 2. Aplicar rotaciones básicas (Álgebra Lineal)
        float radX = rotacionX * 3.14159f / 180.0f;
        float radY = rotacionY * 3.14159f / 180.0f;

        // Rotar en Y
        float x1 = nx * cos(radY) + nz * sin(radY);
        float z1 = -nx * sin(radY) + nz * cos(radY);
        
        // Rotar en X
        float y2 = ny * cos(radX) - z1 * sin(radX);
        float z2 = ny * sin(radX) + z1 * cos(radX);

        // 3. Proyección de Perspectiva
        float den = distanciaCamara - z2;
        if (den < 0.1f) den = 0.1f; // Clamp de seguridad
        float factorPersp = 1.0f / den;
        
        float finalX = x1 * factorPersp * zoom * (pSize.x / 2.0f);
        float finalY = -y2 * factorPersp * zoom * (pSize.y / 2.0f);

        // Centrar en el área del objeto
        return sf::Vector2f(finalX + pSize.x / 2.0f, finalY + pSize.y / 2.0f);
    }

    void rotar(float dx, float dy) {
        float sensibilidad = 0.5f; 
        rotacionY += dx * sensibilidad; // El movimiento horizontal del mouse rota sobre el eje vertical (Y)
        rotacionX += dy * sensibilidad; // El movimiento vertical del mouse rota sobre el eje horizontal (X)

        // Opcional: Limitar la rotación en X para no "dar la vuelta" completa y marear la cámara
        if (rotacionX > 89.0f) rotacionX = 89.0f;
        if (rotacionX < -89.0f) rotacionX = -89.0f;
    }

    void gestionarEvento(const sf::Event& evento, const sf::RenderWindow& window) {
        // --- ROTACIÓN CON EL MOUSE ---
        if (evento.type == sf::Event::MouseButtonPressed) {
            if (evento.mouseButton.button == sf::Mouse::Left) {
                estaRotando = true;
                ultimaPosMouse = sf::Mouse::getPosition(window);
            }
        }

        if (evento.type == sf::Event::MouseButtonReleased) {
            if (evento.mouseButton.button == sf::Mouse::Left) {
                estaRotando = false;
            }
        }

        if (evento.type == sf::Event::MouseMoved && estaRotando) {
            sf::Vector2i posActual = sf::Mouse::getPosition(window);
            
            float dx = static_cast<float>(posActual.x - ultimaPosMouse.x);
            float dy = static_cast<float>(posActual.y - ultimaPosMouse.y);
            
            rotar(dx, dy);
            
            ultimaPosMouse = posActual;
        }

        // --- ZOOM CON EL SCROLL ---
        if (evento.type == sf::Event::MouseWheelScrolled) {
            if (evento.mouseWheelScroll.delta > 0) zoom *= 1.1f;
            else zoom *= 0.9f;
        }
    }

    void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize) override {
      
        
        
        // dibujarCajaLites(window, states, pSize);

        for (auto& [id, cola] : datosSeries) {
            size_t n = cola.size();
            if (n < 2) continue;

            sf::VertexArray linea(sf::LineStrip, n);
            sf::Color col = coloresSeries[id];

            for (size_t i = 0; i < n; ++i) {
                sf::Vector3f p3 = cola[i];
                linea[i].position = proyectar(p3, pSize);
                
                // Efecto de profundidad: oscurecer si está lejos
                float factorZ = (p3.z - lim.minZ) / (lim.maxZ - lim.minZ + 0.001f);
                linea[i].color = sf::Color(col.r, col.g, col.b, 150 + 105 * factorZ);
            }
            window.draw(linea, states);
        }
    }

private:
    float distanciaCamara = 3.0f;
    void dibujarCajaLites(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize);
};



} // namespace dsv
#endif 