/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * - - - - - - -  -  DynSysVis  - - - - - - - - - - 
    * Dynamical System Visualizer Real-Time
    * libreria de herramientas graficas para monitoreo de datos 
    * y comportamiento de sistemas complejos en tiempo Real.
*/
/*  Tablero.hpp
    tiene 
*/

#ifndef TABLERO_HPP
#define TABLERO_HPP

#include "Vista.hpp"


#include <SFML/Graphics.hpp>

namespace dsv {

class Tablero {
     private:
        sf::RenderWindow& window;
        std::vector<std::unique_ptr<Panel>> paneles;
        
        sf::Color colorFondo1;
        sf::Color colorFondo2;
        sf::VertexArray fondo;

        void dibujarFondoEspecial(); // Función auxiliar
    
    public:
        
        Tablero( sf::RenderWindow& ventana,  
                 sf::Color color1 = sf::Color(50, 50, 60),
                 sf::Color color2 = sf::Color(30, 30, 40)  );
        
        
        void setFondoDegradado( sf::Color color1, sf::Color color2);
        
             
        void draw();

        // TODO, mejorar esta cosa
        template<typename T, typename... Args>
        Vista<T> addCompleto( const std::string& titulo, sf::Color color, Args&&... args){
            
            auto p = std::make_unique<Panel>(window, titulo, color);
            T* graf = p->crearContenido<T>(std::forward<Args>(args)...);
            
            Panel& refPanel = *p;
            paneles.push_back(std::move(p));

            Vista<T>  ans =   { *graf, refPanel };
            ans.setPosition(Ubicacion::Centro);
            return ans;
        }

        // VERSION "COMPLETA":
        // recive titulo, color, filas y columnas 
        template<typename T, typename... Args>
        Vista<T> add(const std::string& titulo, sf::Color color,  Args&&... args) {
            return addCompleto<T>(titulo, color, std::forward<Args>(args)...);
        }

        //! VER COMO PUEDO HACER ESO
        // // VERSION "INTERMEDIA": Con valores por defecto  3 x 2
        // // recibe titulo y los argumentos del OBJETO
        // template<typename T, typename... Args>
        // Vista<T> add(const std::string& titulo, sf::Color color, Args&&... args) {
        //     // llama version mas completa
        //     return addCompleto<T>(titulo, color, 2, 2, std::forward<Args>(args)...);
        // }

        // // VERSION "RAPIDA": defaul: Cian, 3 x2 
        // // recibe  título color  los argumentos del OBJETO
        // template<typename T, typename... Args>
        // Vista<T> add(const std::string& titulo, Args&&... args) {
        //     // llama version mas completa
        //     return addCompleto<T>(titulo, sf::Color::Cyan, 3, 2, std::forward<Args>(args)...);
        // }
   
    };
}

#endif