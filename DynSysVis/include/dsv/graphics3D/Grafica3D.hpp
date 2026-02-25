#ifndef GRAFICA3D_HPP
#define GRAFICA3D_HPP

// SFML y Standard
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <limits>

// DSV
#include "dsv/core/Objeto.hpp"
#include "dsv/dataStructures/GestorSeries.hpp"
#include "dsv/graphics3D/Auxiliares3D.hpp" // Para Limites3D, Ejes3D, etc.
#include "dsv/geodraw/draw.hpp"

namespace dsv {

class Grafica3D : public Objeto {
protected:
    GestorSeries gestor;
    Camara3D camara;
    Limites3D lim;
    // Ejes3D ejes; // Se activara cuando definamos la clase de ejes

    void actualizarLimites(float x, float y, float z ){ 
        if (x < lim.minX) lim.minX = x; 
        if (x > lim.maxX) lim.maxX = x;

        if (y < lim.minY) lim.minY = y; 
        if (y > lim.maxY) lim.maxY = y;

        if (z < lim.minZ) lim.minZ = z; 
        if (z > lim.maxZ) lim.maxZ = z;
    }

public:
    Grafica3D( ){ 
        // Inicializar limites para el auto-ajuste de escala
        lim.minX = lim.minY = lim.minZ = std::numeric_limits<float>::max();
        lim.maxX = lim.maxY = lim.maxZ = std::numeric_limits<float>::lowest();
    }

    // ============================================================
    // --- GESTION DE SERIES (API UNIFICADA) --- // TODO <----------
    // ============================================================
    
    // // --- --- --- Gestion de Memoria --- --- ---
    // void setMaxPointsSeries(size_t n){ 
    //     gestor.setMaxPointsSeries(n); 
    // }


    // // --- --- --- Agregar Serie--- --- ---
    // // paleta
    // size_t agregarSerie(const std::string& nombre, const std::vector<sf::Color>& paleta ){ 
    //     return gestor.agregarSerie(nombre, paleta);
    // }

    // size_t agregarSerie(const std::string& nombre, sf::Color col = sf::Color::Cyan ){ 
    //     return gestor.agregarSerie(nombre, col);
    // }
    
    // size_t agregarSerie(sf::Color col = sf::Color::Cyan ){ 
    //     return gestor.agregarSerie(col);
    // }

    // --- Push Back (Alimentacion de datos) ---
    // El nombre es opcional //  si no se pone, va  "default"
    void push_back(float x, float y, float z, const std::string& nombre = "default" ){ 
        gestor.push_back({x, y, z}, nombre);
        actualizarLimites(x, y, z);
    }
    void push_back(float x, float y, float z, size_t id ){ 
        gestor.push_back({x, y, z}, id);
        actualizarLimites(x, y, z);
    }

    // // --- Set Color (Unificado: acepta Color o Paleta) ---
    
    // // Por Nombre
    // void setColor(const std::string& nombre, sf::Color col ){  gestor.setColor(col, nombre); }
    // void setColor(const std::string& nombre, const std::vector<sf::Color>& paleta ){  gestor.setColor(paleta, nombre); }

    // // Por ID
    // void setColor(size_t id, sf::Color col ){  gestor.setColor(col, id); }
    // void setColor(size_t id, const std::vector<sf::Color>& paleta ){  gestor.setColor(paleta, id); }

    // // Global (Para todas las series)
    // void setColor(sf::Color col ){  gestor.setColorSeries(col); }
    // void setColor(const std::vector<sf::Color>& paleta ){  gestor.setColorSeries(paleta); }

    
    //  // --- --- ---  Grosores --- --- --- 
    // void setGrosor(float g, size_t id){}
    // void setGrosor(float g, const std::string& nombre= "default");
    // void setGrosorSeries(float g);


    // // --- Difuminado ---
    // void setDifuminado(bool b, size_t id);
    // void setDifuminado(bool b, const std::string& nombre= "default");
    // void setDifuminadoSeries(bool b);



    // ============================================================
    // --- CONTROL Y RENDER ---
    // ============================================================

    void gestionarEvento(const sf::Event& ev, const sf::RenderWindow& win ){ 
        camara.gestionarEvento(ev, win);
    }

    void draw(sf::RenderWindow& window, sf::RenderStates states, sf::Vector2f pSize) override {
        // 1. Dibujar Ejes y Rejilla (Pendiente de implementar en Auxiliares3D)
        // ejes.draw(window, states, camara, lim, pSize);

        // 2. Dibujar Series
        for( const auto& serie : gestor.lista ){ 
            if (serie.count < 2) continue;

            // Proyeccion de puntos del buffer circular
            std::vector<sf::Vector2f> puntos2D;
            puntos2D.reserve(serie.count);

            for( size_t i = 0; i < serie.count; ++i ){ 
                sf::Vector3f p3 = serie.getPunto3D(i);
                puntos2D.push_back(camara.proyectar(p3, pSize, lim));
            }

            // Grosor reactivo al zoom
            float grosorFinal = serie.grosor * (camara.zoom * 0.5f);
            if (grosorFinal < 1.0f) grosorFinal = 1.0f;
            bool grosorDinamico = serie.adelgazarCola;

            if( serie.paleta.empty() ){
                DSV_LOG_ERROR("has olvidado asignar un color o paleta a una serie ");
            }

            // paleta base
            std::vector<sf::Color> paletaRender = serie.paleta;

            if( serie.difuminarCola ){
                // Si es un color unico, lo duplicamos para poder hacer el degradado
                if( paletaRender.size() == 1 ){
                    for( int i=0; i<10; i++)
                        paletaRender.push_back(paletaRender[0]);
                }

                // aplicamos el fade linealmente
                size_t n = paletaRender.size();
                for( size_t i = 0; i < n; ++i ){
                    float factor = static_cast<float>(i) / (n - 1);
                    paletaRender[i].a = static_cast<sf::Uint8>(paletaRender[i].a * factor);
                }
            }

            // Dibujamos (si no entro al if, dibuja la paleta original intacta)
            dsv::draw::linea(window, states, puntos2D, paletaRender, grosorFinal, grosorDinamico);

        }
    }

    // Accesos para configuracion fina
    Camara3D& getCamara( ){  return camara; }
    GestorSeries& getGestor( ){  return gestor; }
};

} // namespace dsv

#endif