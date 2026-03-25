/*
    CONTIENE
        -> ejes 3d (gestiona grosor, color, min max, dibujado)

*/
#ifndef AUX3D_HPP
#define AUX3D_HPP

//  sfml y std
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <deque>
#include <algorithm> // para min y max

#include <dsv/core/Logger.hpp>

namespace dsv{

/*
    --- --- --- --- --- --- --- --- --- --- --- 
    --- --- - ELEMENTOS DE LA GRAFICA - --- --- 
    --- --- --- --- --- --- --- --- --- --- --- 
*/  

struct Limites3D {
    float minX, maxX, minY, maxY, minZ, maxZ;

    void setLimites(float mx, float Mx, float my, float My, float mz, float Mz ){ 
        minX = mx; maxX = Mx;
        minY = my; maxY = My;
        minZ = mz; maxZ = Mz;
    }

    void actualiza( float mx, float Mx, float my, float My, float mz, float Mz ){ 

        auto act = [](float& a, float b, bool esMinimo) { 
            if (esMinimo && b < a) a = b; // actualiza si el nuevo es más pequeño
            if (!esMinimo && b > a) a = b; // actualiza si el nuevo es más grande
        };

        act(minX, mx, true);  act(maxX, Mx, false);
        act(minY, my, true);  act(maxY, My, false);
        act(minZ, mz, true);  act(maxZ, Mz, false);
    }
};



struct Ejes3D { 
    // limites ejes
    float minX = -20.f, maxX = 20.f;
    float minY = -20.f, maxY = 20.f;
    float minZ = -20.f, maxZ = 20.f;

    sf::Color colorX = sf::Color::Red;
    sf::Color colorY = sf::Color::Green;
    sf::Color colorZ = sf::Color::Blue;

    bool visible = true;
    float grosor = 1.0f;

    // set Todos
    void setLimites( float m, float M){
        setLimites(m,M, m,M, m,M);
    }
    void setLimites(float mx, float Mx, float my, float My, float mz, float Mz ){ 
        minX = mx; maxX = Mx;
        minY = my; maxY = My;
        minZ = mz; maxZ = Mz;
    }

    // set Individual
    void setEjeXLimites(float min, float max ){  minX = min; maxX = max; }
    void setEjeYLimites(float min, float max ){  minY = min; maxY = max; }
    void setEjeZLimites(float min, float max ){  minZ = min; maxZ = max; }
};




/*
    --- --- --- --- --- --- --- --- --- --- --- 
    --- --- --- CAMARA Y PROYECCION --- --- --- 
    --- --- --- --- --- --- --- --- --- --- --- 
*/  

class Camara3D {
public:
    float rotacionX = 45.0f;
    float rotacionY = 45.0f;
    float distanciaCamara = 50.0f;
    float sensibilidad = 0.4f;
    
    // limites internos y traslacion
    float centroX = 0.0f;
    float centroY = 0.0f;
    float centroZ = 0.0f;
    
    float zoom = 1.0f;


    // posicion
    sf::FloatRect ultimoBoundsGlobal; 

    // mouse
    sf::Vector2i ultimaPosMouse;
    bool estaRotando = false;
    bool estaTrasladando = false;

    void rotar(float dx, float dy) { 
        rotacionY += dx * sensibilidad; 
        rotacionX += dy * sensibilidad; 

        // limitar rotaciones para no marear al usuario
        if( rotacionX > 89.0f)  rotacionX = 89.0f;
        if( rotacionX < -89.0f) rotacionX = -89.0f;
    }

    void trasladar(float dx, float dy){
        // La velocidad de traslacion debe ser proporcional al rango visible
        float factorMovimiento = zoom * 0.02f; 
        
        // Traslacion simple basada en la rotacion Y actual para que se sienta intuitivo
        float radY = rotacionY * 3.14159f / 180.0f;
        
        centroX -= (dx * cos(radY) - dy * sin(radY) * 0.5f) * factorMovimiento;
        centroZ -= (dx * sin(radY) + dy * cos(radY) * 0.5f) * factorMovimiento;
        centroY += dy * factorMovimiento; // Movimiento vertical
    }

  // --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
    // --- --- --- --- --- --- 3D -> 2d --- --- --- --- --- --- 
    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- 

    // Convertimos un punto del Mundo 3D al "Espacio de la Cámara"
    // Nota: La hice 'const' para que pueda ser llamada desde 'proyectar'
    sf::Vector3f normalizarPunto(sf::Vector3f p) const {
        // 1. Trasladar: Hacemos que la cámara sea el centro del universo temporalmente
        float dx = p.x - centroX;
        float dy = p.y - centroY;
        float dz = p.z - centroZ;

        // 2. Escalado Uniforme (Zoom estilo GeoGebra)
        // Al dividir entre un zoom pequeño (ej. 0.5), el valor se multiplica (x2), 
        // haciendo que el objeto se vea más grande en pantalla.
        dx /= zoom;
        dy /= zoom;
        dz /= zoom;

        // 3. Convertir los grados de rotación a radianes
        float radX = rotacionX * 3.14159f / 180.0f;
        float radY = rotacionY * 3.14159f / 180.0f;

        // 4. Rotación en el eje Y (Yaw)
        float x1 = dx * std::cos(radY) - dz * std::sin(radY);
        float z1 = dx * std::sin(radY) + dz * std::cos(radY);

        // 5. Rotación en el eje X (Pitch)
        float y2 = dy * std::cos(radX) - z1 * std::sin(radX);
        float z2 = dy * std::sin(radX) + z1 * std::cos(radX);

        // Retornamos el punto completamente transformado
        return sf::Vector3f(x1, y2, z2);
    }

    sf::Vector2f proyectar(sf::Vector3f p, sf::Vector2f pSize) const {
        // 1. Pasar por el proceso de normalización (Traslación + Escala + Rotación)
        sf::Vector3f pCamara = normalizarPunto(p);

        // 2. Alejar el punto usando la distancia de la cámara 
        // (Para que no esté pegado a nuestra "cara" virtual)
        float zFinal = pCamara.z + distanciaCamara;

        // 3. Recorte (Clipping)
        // Si el punto queda detrás de nosotros, devolvemos un punto fuera de la pantalla
        if (zFinal <= 0.1f) {
            return sf::Vector2f(-100000.0f, -100000.0f);
        }

        // 4. Proyección de Perspectiva (3D a 2D)
        // Usamos un factor de FOV fijo. 500.0f suele verse muy natural.
        float fovScale = 500.0f; 
        float xProj = (pCamara.x / zFinal) * fovScale;
        
        // IMPORTANTE: SFML dibuja el eje Y hacia abajo (el 0 está arriba).
        // En matemáticas, Y va hacia arriba. Ponemos un signo negativo para invertirlo 
        // y que tus gráficas no salgan de cabeza.
        float yProj = (-pCamara.y / zFinal) * fovScale;

        // 5. Desplazar el resultado al centro del área designada (pSize)
        return sf::Vector2f(xProj + pSize.x / 2.0f, yProj + pSize.y / 2.0f);
    }

    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    // --- --- --- --- --- --- Gestionar Evento --- --- --- --- --- --- ---
    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    void gestionarEvento(const sf::Event& evento, const sf::RenderWindow& window) {
        sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePosI);
        
        // Si el mouse no esta dentro de los bounds globales, no procesamos clics ni scroll
        if (!ultimoBoundsGlobal.contains(mousePos)) {
            // Pero si ya estabamos rotando/trasladando, permitimos que termine aunque salga del area
            if (evento.type != sf::Event::MouseMoved && evento.type != sf::Event::MouseButtonReleased)
                return;
        }

        // presionar botones
        if (evento.type == sf::Event::MouseButtonPressed) {
            if (evento.mouseButton.button == sf::Mouse::Left) {
                estaRotando = true;
                ultimaPosMouse = sf::Mouse::getPosition(window);
            }
            if (evento.mouseButton.button == sf::Mouse::Right) {
                estaTrasladando = true;
                ultimaPosMouse = sf::Mouse::getPosition(window);
            }
        }

        // soltar botones
        if (evento.type == sf::Event::MouseButtonReleased) {
            if (evento.mouseButton.button == sf::Mouse::Left) estaRotando = false;
            if (evento.mouseButton.button == sf::Mouse::Right) estaTrasladando = false;
        }

        // moviendo el mouse (rotar o trasladar)
        if (evento.type == sf::Event::MouseMoved) {
            sf::Vector2i posActual = sf::Mouse::getPosition(window);
            float dx = static_cast<float>(posActual.x - ultimaPosMouse.x);
            float dy = static_cast<float>(posActual.y - ultimaPosMouse.y);

            if (estaRotando) {
                rotar(dx, dy);
            } else if (estaTrasladando) {
                trasladar(zoom*dx, zoom*dy);
            }
            
            ultimaPosMouse = posActual;
        }

        // zoom con la rueda del mouse no se exactamente que debe cambiar
        if (evento.type == sf::Event::MouseWheelScrolled) {
            if (evento.mouseWheelScroll.delta > 0){
                zoom *= 0.9f; // acercar
            } else {
                zoom *= 1.1f; // alejar
            }
            
            // limites de seguridad para el rango
            if(zoom < 0.001f) zoom = 0.001f;
            if(zoom > 1000.0f) zoom = 1000.0f;
        }
    }
};

}
#endif