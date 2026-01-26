/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: InsightRT - - - - - - - - - - - - - - - - - - - 
    * libreria de herramientas graficas para monitoreo de datos 
    * en vivo y comportamiento de sistemas complejos.
 */
/*  GEOMETRIA.cpp
    sistema de clases base para graficar
    clases derivadas: para la visualización de series temporales  
                      para retratos de fase.
*/
#include "Graficas.hpp"

/*
    GRAFICAS BASE
    aquiv an lso metodos que deberian tener todas las graficas en comun
*/
GraficaBase::GraficaBase(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente) 
    : maxPoints(maxPts), lineaResaltado(color), titulo(t ){ 
    if( !font.loadFromFile(ruta_fuente) ){  /* error */ }

    // medidas
    tamanoTitulo = 12.f;
    titulo_texto.setFont(font);
    titulo_texto.setString(titulo);
    titulo_texto.setCharacterSize(tamanoTitulo);
    titulo_texto.setFillColor(sf::Color::White);

    nombreEjeX = "Eje X"; nombreEjeY = "Eje Y";
    unidadEjeX = "u"; unidadEjeY = "u";
    numMarcasX = 5; numMarcasY = 4;
    mostrarEtiquetasEjes = false;
}

void GraficaBase::draw(sf::RenderWindow& window, Panel& parent ){ 
    float alturaTitulo = titulo_texto.getGlobalBounds().height;
    sf::Vector2f pSize = parent.getSize();
    sf::RenderStates states = parent.getInternalState();
    
    float paddingL = mostrarEtiquetasEjes ? 45.f : 30.f;
    float paddingB = mostrarEtiquetasEjes ? 35.f : 25.f;
    float graphWidth = pSize.x - (paddingL + 20.f);
    float offsetTop = alturaTitulo + 25.f;
    float graphHeight = pSize.y - (paddingB + offsetTop * 1.0);

    titulo_texto.setPosition(15.f, alturaTitulo - 3.0f);
    window.draw(titulo_texto, states);

    sf::Color colorGuia(60, 60, 60); 
    sf::Color axisColor(100, 100, 100);

    // --- marcas Y con etiquetas ---
    for( int i = 0; i < numMarcasY; i++ ){ 
        float fraccion = (float)i / (numMarcasY - 1);
        float yPos = offsetTop + graphHeight - (fraccion * graphHeight);

        sf::VertexArray lineaGuia(sf::Lines, 2);
        lineaGuia[0] = { {paddingL, yPos}, colorGuia };
        lineaGuia[1] = { {paddingL + graphWidth, yPos}, colorGuia };
        window.draw(lineaGuia, states);

        // AQUI LLAMAMOS A LOS METODOS DE LA GRAFICA HIJA
        sf::Text label(getEtiquetaY(i), font, 10);
        label.setPosition(paddingL - 25.f, yPos - 7.f);
        label.setFillColor(axisColor);
        window.draw(label, states);
    }

    // --- marcas X con etiquetas ---
    for( int i = 0; i < numMarcasX; i++ ){ 
        float fraccion = (float)i / (numMarcasX - 1);
        float xPos = paddingL + (fraccion * graphWidth);

        sf::VertexArray lineaGuia(sf::Lines, 2);
        lineaGuia[0] = { {xPos, offsetTop}, colorGuia };
        lineaGuia[1] = { {xPos, offsetTop + graphHeight}, colorGuia };
        window.draw(lineaGuia, states);

         // AQUI LLAMAMOS A LOS METODOS DE LA GRAFICA HIJA
        sf::Text label(getEtiquetaX(i), font, 10);
        label.setPosition(xPos - 5.f, offsetTop + graphHeight + 5.f);
        label.setFillColor(axisColor);
        window.draw(label, states);
    }

    // etiquetas de los ejes
    if( mostrarEtiquetasEjes ){ // por lo general no uso esta parte
        sf::Text txtEjeY(nombreEjeY + " (" + unidadEjeY + ")", font, 11);
        txtEjeY.setPosition(5.f, offsetTop - 20.f);
        txtEjeY.setFillColor(sf::Color::Cyan);
        window.draw(txtEjeY, states);

        sf::Text txtEjeX(nombreEjeX + " (" + unidadEjeX + ")", font, 11);
        txtEjeX.setPosition(paddingL + graphWidth - 80.f, offsetTop + graphHeight + 18.f);
        txtEjeX.setFillColor(sf::Color::Cyan);
        window.draw(txtEjeX, states);
    }

    // --- colorcar --- 
    sf::VertexArray ejes(sf::Lines, 4);
    ejes[0] = { {paddingL, offsetTop}, axisColor };
    ejes[1] = { {paddingL, offsetTop + graphHeight}, axisColor };
    ejes[2] = { {paddingL, offsetTop + graphHeight}, axisColor };
    ejes[3] = { {paddingL + graphWidth, offsetTop + graphHeight}, axisColor };
    window.draw(ejes, states);

    dibujarContenido(window, states, paddingL, offsetTop, graphWidth, graphHeight);
}


/*
    GRAFICAS TIEMPO
   el eje x es el tiempo y vamos agtregando datos
   se supone que los datos se van agregando en tiempo real
*/
GraficaTiempo::GraficaTiempo(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente)
    : GraficaBase(maxPts, color, t, ruta_fuente ), sombreado(true) { }

void GraficaTiempo::addValue(float val ){ 
    datos.push_back(val);
    if( datos.size() > maxPoints) datos.erase(datos.begin());
}

std::string GraficaTiempo::getEtiquetaY(int i ){ 
    if( datos.empty()) return "0";
    float maxValue = *std::max_element(datos.begin(), datos.end());
    if( maxValue < 1.0f) maxValue = 1.0f;
    float fraccion = (float)i / (numMarcasY - 1);
    return std::to_string((int)(maxValue * fraccion));
}

std::string GraficaTiempo::getEtiquetaX(int i ){ 
    return std::to_string(i); 
}

void GraficaTiempo::dibujarContenido(sf::RenderWindow& window, sf::RenderStates states, float paddingL, float offsetTop, float graphWidth, float graphHeight ){ 
    if( datos.empty()) return;

    float maxValue = *std::max_element(datos.begin(), datos.end());
    if( maxValue < 1.0f) maxValue = 1.0f;

    float xStep = graphWidth / static_cast<float>(maxPoints - 1);
    float ejeYBase = offsetTop + graphHeight;

    // --- efecto de sombreado (vertical gradient fill) ---
    if( sombreado ){
        sf::VertexArray degradado(sf::TriangleStrip, datos.size() * 2);
        for( size_t i = 0; i < datos.size(); i++ ){
            float x = paddingL + (i * xStep);
            float y = ejeYBase - (datos[i] / maxValue * graphHeight);

            // vertice arriba (color con transparencia)
            degradado[2 * i].position = {x, y};
            degradado[2 * i].color = sf::Color(lineaResaltado.r, lineaResaltado.g, lineaResaltado.b, 80);

            // vertice abajo (en el eje con transparencia total)
            degradado[2 * i + 1].position = {x, ejeYBase};
            degradado[2 * i + 1].color = sf::Color(lineaResaltado.r, lineaResaltado.g, lineaResaltado.b, 0);
        }
        window.draw(degradado, states);
    }

    // --- linea principal (glowing top line) ---
    sf::VertexArray linea(sf::LineStrip, datos.size());
    for( size_t i = 0; i < datos.size(); i++ ){ 
        float x = paddingL + (i * xStep);
        float y = ejeYBase - (datos[i] / maxValue * graphHeight);
        linea[i].position = {x, y};
        linea[i].color = lineaResaltado;
    }
    window.draw(linea, states);
}

void GraficaTiempo::ponerSobreado( bool s ){
    sombreado = s;
}

/*
    GRAFICAS DE ESPACIO FASE
    agrega pares
*/
GraficaEspacioFase::GraficaEspacioFase(unsigned int maxPts, sf::Color color, std::string t, const std::string& ruta_fuente)
    : GraficaBase(maxPts, color, t, ruta_fuente ){ 
    nombreEjeX = "X";
    nombreEjeY = "Y";
}

void GraficaEspacioFase::addValue(float x, float y ){ 
    puntos.push_back({x, y});
    if( puntos.size() > maxPoints ){ 
        puntos.erase(puntos.begin());
    }
}

std::string GraficaEspacioFase::getEtiquetaY(int i ){ 
    if( puntos.empty()) return "0";
    float maxY = 0.01f;
    for(auto& p : puntos) if(p.y > maxY) maxY = p.y;
    
    float fraccion = (float)i / (numMarcasY - 1);
    return std::to_string((int)(maxY * fraccion));
}

std::string GraficaEspacioFase::getEtiquetaX(int i ){ 
    if( puntos.empty()) return "0";
    float maxX = 0.01f;
    for(auto& p : puntos) if(p.x > maxX) maxX = p.x;
    
    float fraccion = (float)i / (numMarcasX - 1);
    return std::to_string((int)(maxX * fraccion));
}

void GraficaEspacioFase::dibujarContenido(sf::RenderWindow& window, sf::RenderStates states, float paddingL, float offsetTop, float graphWidth, float graphHeight ){ 
    if( puntos.empty()) return;

    float maxX = 0.01f;
    float maxY = 0.01f;
    for( auto& p : puntos ){ 
        if( p.x > maxX) maxX = p.x;
        if( p.y > maxY) maxY = p.y;
    }
    maxX *= 1.2, maxY *= 1.2;
    
    sf::VertexArray linea(sf::LineStrip, puntos.size());

    for( size_t i = 0; i < puntos.size(); i++ ){ 
        float posX = paddingL + (puntos[i].x / maxX * graphWidth);
        float posY = offsetTop + graphHeight - (puntos[i].y / maxY * graphHeight);
        
        linea[i].position = {posX, posY};
        linea[i].color = lineaResaltado;
    }
    window.draw(linea, states);
}