/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: InsightRT - - - - - - - - - - - - - - - - - - - 
    * libreria de herramientas graficas para monitoreo de datos 
    * en vivo y comportamiento de sistemas complejos.
*/
/*  GEOMETRIA.hpp
    sistema de clases base para graficar
    clases derivadas: para la visualización de series temporales  
                      para retratos de fase.
*/
#ifndef GRAFICAS_HPP
#define GRAFICAS_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include "Geometria.hpp" // Para que reconozca la clase Panel

class GraficaBase {
protected:
    unsigned int maxPoints;
    sf::Color lineaResaltado;
    sf::Font font;
    sf::Text titulo_texto;
    std::string titulo;

    std::string nombreEjeX, nombreEjeY;
    std::string unidadEjeX, unidadEjeY;
    int numMarcasX, numMarcasY;
    
    bool mostrarEtiquetasEjes;
    float tamanoTitulo;

public:
    GraficaBase(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente = "fonts/Roboto.ttf");

    virtual ~GraficaBase() {}

    // IMPORTANTE: metodos que las graficas  hija DEBE implementar 
    virtual std::string getEtiquetaY(int indice) = 0;
    virtual std::string getEtiquetaX(int indice) = 0;
    virtual void dibujarContenido(sf::RenderWindow& window, sf::RenderStates states, float paddingL, float offsetTop, float graphWidth, float graphHeight) = 0;

    // --- estilo ---
    void setMostrarEtiquetas(bool mostrar) { mostrarEtiquetasEjes = mostrar; }
    void setTamanoTitulo(float tam) { tamanoTitulo = tam; titulo_texto.setCharacterSize((unsigned int)tamanoTitulo); }
    void configurarEjes(std::string nx, std::string ux, std::string ny, std::string uy) { nombreEjeX = nx; unidadEjeX = ux; nombreEjeY = ny; unidadEjeY = uy; }
    void configurarMarcas(int mx, int my) { numMarcasX = mx; numMarcasY = my; }

    void draw(sf::RenderWindow& window, Panel& parent);
};

class GraficaTiempo : public GraficaBase {
private:
    std::vector<float> datos;
    float contadorSegundos;
    bool sombreado;

public:
    GraficaTiempo(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente = "fonts/Roboto.ttf");

    void addValue(float val);

    // ---  etiquetas para tiempo y eje y---
    std::string getEtiquetaY(int i) override;
    std::string getEtiquetaX(int i) override;

    // --- aqui en la grafica hija se completa --- 
    void dibujarContenido(sf::RenderWindow& window, sf::RenderStates states, float paddingL, float offsetTop, float graphWidth, float graphHeight) override;

    // estilo
    void ponerSobreado( bool s );
};

class GraficaEspacioFase : public GraficaBase {
private:
    std::vector<sf::Vector2f> puntos; //  pares (x, y)

public:
    GraficaEspacioFase(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente = "fonts/Roboto.ttf");

    // --- datos ---
    void addValue(float x, float y);

    // --- etiquetas para espacio Fase ---
    std::string getEtiquetaY(int i) override;
    std::string getEtiquetaX(int i) override;

    // --- dibujo ---
    void dibujarContenido(sf::RenderWindow& window, sf::RenderStates states, float paddingL, float offsetTop, float graphWidth, float graphHeight) override;
};

#endif