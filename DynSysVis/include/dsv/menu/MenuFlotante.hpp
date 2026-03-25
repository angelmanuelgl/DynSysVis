#ifndef MENUFLOTANTE_HPP
#define MENUFLOTANTE_HPP

#include <vector>
#include <memory>

#include "dsv/core/Logger.hpp"
#include "dsv/core/Panel.hpp"
#include "dsv/menu/CampoControl.hpp"

namespace dsv {

class MenuFlotante : public Objeto {
public:
    explicit MenuFlotante(float espaciadoV = 4.f, float padding = 10.f)
        : espaciadoV(espaciadoV), padding(padding ){}

    // ── agregar filas ────────────────────────────────────────────
    CampoControl& agregarFila(float altoFila = 30.f ){
        filas.push_back(std::make_unique<CampoControl>(altoFila));
        return *filas.back();
    }

    // ── Objeto interface ─────────────────────────────────────────

    // el PanelFlotante llama esto para saber cuanto espacio necesita
    sf::Vector2f getSize() const override {
        return { getAnchoTotal(), getAltoTotal() };
    }

    void draw(sf::RenderWindow& window,
              sf::RenderStates  states,
              sf::Vector2f      pSize) override
    {       
        
        actualizarBoundsGlobal(states, pSize);

        // extraer posicion del transform de states
        sf::Vector2f origen = {
            states.transform.transformPoint({0,0}).x,
            states.transform.transformPoint({0,0}).y
        };

        float y = origen.y + padding;
        for(auto& fila : filas ){
            fila->draw(window, {origen.x + padding, y}, Objeto::fuenteCompartida);
            y += fila->getAltoNecesario() + espaciadoV;
        }
    }

    // ── eventos — el PanelFlotante los pasa aqui ─────────────────
    void onMouseMove(sf::Vector2f mp, sf::Vector2f origen ){
        distribuir([&](CampoControl& f, sf::Vector2f p){ f.onMouseMove(mp, p); }, origen);
    }
    void onMousePress  (sf::Vector2f mp, sf::Vector2f origen ){
        distribuir([&](CampoControl& f, sf::Vector2f p){ f.onMousePress(mp, p); }, origen);
    }
    void onMouseRelease(sf::Vector2f mp, sf::Vector2f origen ){
        distribuir([&](CampoControl& f, sf::Vector2f p){ f.onMouseRelease(mp, p); }, origen);
    }


    void gestionarEvento(const sf::Event& ev, const sf::RenderWindow& win) override {
        // 1. Obtener posición del mouse transformada
        sf::Vector2i mousePosI = sf::Mouse::getPosition(win);
        sf::Vector2f mousePos = win.mapPixelToCoords(mousePosI);
        
        if( !ultimoBoundsGlobal.contains(mousePos)) return;

        // El origen real para los hijos es la esquina superior izquierda del bounds global
        sf::Vector2f origen = { ultimoBoundsGlobal.left, ultimoBoundsGlobal.top };

        // Filtro de seguridad (opcional para MouseMove si quieres hover fuera, 
        // pero obligatorio para clicks)
        bool mouseAdentro = ultimoBoundsGlobal.contains(mousePos);

        switch( ev.type ){
            case sf::Event::MouseMoved:
                // para que que el hover funcione, distribuimos siempre
                if( mouseAdentro ){
                    this->onMouseMove(mousePos, origen);
                }
                break;

            case sf::Event::MouseButtonPressed:
                if( mouseAdentro && ev.mouseButton.button == sf::Mouse::Left ){
                   this->onMousePress(mousePos, origen);
                }
                break;

            case sf::Event::MouseButtonReleased:
                // Soltar el click a veces se gestiona aunque estés fuera (drag & drop)
                if( ev.mouseButton.button == sf::Mouse::Left ){
                    this->onMouseRelease(mousePos, origen);
                }
                break;

            default:
                break;
        }
    }

private:
    float getAnchoTotal() const {
        float maxAncho = 0.f;
        for(const auto& f : filas)
            maxAncho = std::max(maxAncho, f->getAnchoNecesario());
        return maxAncho + padding * 2;
    }

    float getAltoTotal() const {
        float alto = padding * 2;
        for(const auto& f : filas)
            alto += f->getAltoNecesario() + espaciadoV;
        return alto > padding * 2 ? alto - espaciadoV : alto;  // quitar ultimo espaciado
    }

    template<typename Fn>
    void distribuir(Fn fn, sf::Vector2f origen ){
        float y = origen.y + padding;
        for(auto& f : filas ){
            fn(*f, {origen.x + padding, y});
            y += f->getAltoNecesario() + espaciadoV;
        }
    }

    std::vector<std::unique_ptr<CampoControl>> filas;
    float espaciadoV;
    float padding;
};

} // namespace dsv
#endif