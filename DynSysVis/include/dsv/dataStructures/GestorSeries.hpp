/*
`   CONTINENE: el gestor de series
    -> ajustar color
    -> ajsutar paleta
    -> acceder por indice
    -> acceder por nombre
    -> push back
*/

#ifndef GESTORSERIES_HPP
#define GESTORSERIES_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>

#include "dsv/dataStructures/Serie.hpp"
#include "dsv/interface/Temas.hpp"

namespace dsv {

struct GestorSeries {
    std::vector<Serie> lista;
    std::unordered_map<std::string, size_t> mapaNombres;
    
    // Configuracion global por defecto para nuevas series
    int dimensionGlobal = 3;
    size_t maxPointsGlobal = 3000;

    GestorSeries();
    
    // Configuracion global por defecto para nuevas series
    float grosor_global = 1.0f; 
    std::vector<sf::Color> paleta_global = dsv::Color::Arcoiris();
    bool difuminar_global = true;
    bool adelgazar_global = true;
    

    // centralizar el estilo acutal
    void aplicarEstiloBase(Serie& s) {
        s.grosor = grosor_global;
        s.paleta = paleta_global; // se supone queeeee al inizializar no queremos sobreescribir el color que elija
        s.difuminarCola = difuminar_global;
        s.adelgazarCola = adelgazar_global;
    }


    // --- --- --- Gestion de Memoria --- --- ---
    void reajustarCapacidad(size_t idNecesario);
    // Max Points Global (Para series futuras y actuales) 
    void setMaxPointsSeries(size_t n);
    
    // --- --- --- Agregar Serie--- --- ---
    // paleta
    size_t agregarSerie(const std::string& nombre, const std::vector<sf::Color>& paleta );
    size_t agregarSerie(size_t id, const std::vector<sf::Color>& paleta);
    // color
    size_t agregarSerie(const std::string& nombre, sf::Color col);
    size_t agregarSerie(size_t id, sf::Color col);
    // paleta por defecto
    size_t agregarSerie(const std::string& nombre);
    size_t agregarSerie(size_t id);

    

    


    // --- --- --- Push Back --- --- ---
    // Ahora usamos std::vector<float> para soportar cualquier dimension
    void push_back(const std::vector<float>& punto, size_t id);
    void push_back(const std::vector<float>& punto, const std::string& nombre = "default");

    // --- --- --- Colores --- --- ---
    // por color
    void setColor(sf::Color col, size_t id);
    void setColor(sf::Color col, const std::string& nombre = "default");
    void setColorSeries(sf::Color col);

    // --- --- --- Paletas --- --- ---
    void setColor(const  std::vector<sf::Color>& paleta, size_t id);
    void setColor(const  std::vector<sf::Color>& paleta, const std::string& nombre);
    void setColorSeries(const  std::vector<sf::Color>& paleta);
    
   
    // --- --- ---  Grosores --- --- --- 
    void setGrosor(float g, size_t id);
    void setGrosor(float g, const std::string& nombre= "default");
    void setGrosorSeries(float g);


    // --- Difuminado ---
    void setDifuminado(bool b, size_t id);
    void setDifuminado(bool b, const std::string& nombre= "default");
    void setDifuminadoSeries(bool b);

    void setAdelgazado(bool b, size_t id);
    void setAdelgazado(bool b, const std::string& nombre= "default");
    void setAdelgazadoSeries(bool b);
    
    // --- --- --- Acceso rapido --- --- ---
    Serie& operator[](const std::string& nombre);
    Serie& operator[](size_t id);


    
};

} // dsv
#endif