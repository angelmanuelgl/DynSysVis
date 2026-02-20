// ** EN CONSTRUCCION

#ifndef GRAFICAND_HPP
#define GRAFICAND_HPP

// std y sfml
#include <deque>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>


// dsv
#include "dsv/core/Logger.hpp"

namespace dsv {

/**
 * CONTENEDOR PARA : trayectorias de N dimensiones.
 * Optimizada para push_back, pop_front y acceso aleatorio.
*/
class SeireND {
private:
    std::deque<std::vector<float>> puntos;
    size_t dimensiones;
    size_t maxPuntos;

public:
    SeireND(size_t dims = 3, size_t maxPts = 5000) 
        : dimensiones(dims), maxPuntos(maxPts) {}


    // Agregar un punto ND
    void push_back(const std::vector<float>& punto) {
        if( punto.size() != dimensiones ){
            DSV_LOG_WARN("no coincide la dimension " + punto.size() + "con" + dimensiones );
            return; 
        }
        puntos.push_back(punto);
        if (puntos.size() > maxPuntos) {
            puntos.pop_front();
        }
    }

    // basicos
    size_t size() const { return puntos.size(); }
    bool empty() const { return puntos.empty(); }
    void clear() { puntos.clear(); }

    // Gets
    const std::vector<float>& operator[](size_t i) const { return puntos[i]; }
    const std::vector<float>& back() const { return puntos.back(); }
    const std::vector<float>& front() const { return puntos.front(); }
    size_t getDimensiones() const { return dimensiones; }

};

/**
    GRAFICA 
*/
class GraficaND : public Objeto {
protected:
    // tener las series
    std::map<std::string, SeireND> seriesND;
    std::map<std::string, sf::Color> coloresSeries;
    
    // --- visualziacion ---
    float zoom = 1.0f;
    float rotacionX = 45.0f;
    float rotacionY = 45.0f;
    
    // indices para proyectar
    int dimX = 0, dimY = 1, dimZ = 2;

public:
    GraficaND(size_t dims=3) : Objeto() {
        // Por defecto creamos la serie principal
        seriesND["default"] = DataStreamND(dims);
    }

    // agregar datos
    void push_back( const std::vector<float>& valores, std::string id = "default", sf::Color col = sf::Color::White ){
        if( seriesND.find(id) == seriesND.end() ){
            seriesND[id] = DataStreamND(valores.size());
        }
        seriesND[id].push(valores);
        coloresSeries[id] = col;
    }

    // para proeyctar
    virtual sf::Vector2f proyectarPunto(const std::vector<float>& pND, sf::Vector2f pSize) = 0;
};


#endif
} // namespace dsv