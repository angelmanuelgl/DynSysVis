

#ifndef CAMPOCONTROL_HPP
#define CAMPOCONTROL_HPP

// std y sfml
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>

// dsv
#include "dsv/core/Logger.hpp"
#include "dsv/menu/Campo.hpp"


namespace dsv {

class CampoControl {
public:
    explicit CampoControl(float altoFila = 30.f) : altoFila(altoFila) {}

    // ── agregar campos ───────────────────────────────────────────
    template<typename T, typename... Args>
    T* agregar(Args&&... args) {
        auto c = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = c.get();
        campos.push_back(std::move(c));
        return ptr;
    }

    // el ultimo campo agregado baja a un nuevo renglon
    void nuevoRenglon() {
        if(!campos.empty())
            campos.back()->nuevoRenglon = true;
    }

    // ── medidas auto ─────────────────────────────────────────────
    float getAnchoNecesario() const {
        float anchoMax = 0.f, anchoLinea = 0.f;
        for(const auto& c : campos) {
            anchoLinea += c->getAncho();
            if(c->nuevoRenglon) {
                anchoMax  = std::max(anchoMax, anchoLinea);
                anchoLinea = 0.f;
            }
        }
        return std::max(anchoMax, anchoLinea);
    }

    float getAltoNecesario() const {
        return renglones() * altoFila;
    }

    // ── dibujar ──────────────────────────────────────────────────
    // pos es la esquina superior izquierda de esta fila completa
    void draw(sf::RenderWindow& window, sf::Vector2f pos, const sf::Font& fuente) {
        float x = pos.x, y = pos.y;
        for(const auto& c : campos) {
            c->draw(window, {x, y}, altoFila, fuente);
            x += c->getAncho();
            if(c->nuevoRenglon) { x = pos.x; y += altoFila; }
        }
    }

    // ── eventos ──────────────────────────────────────────────────
    void onMouseMove   (sf::Vector2f mp, sf::Vector2f posControl) {
        distribuirEvento([&](Campo& c){ c.onMouseMove(mp); }, posControl);
    }
    void onMousePress  (sf::Vector2f mp, sf::Vector2f posControl) {
        distribuirEvento([&](Campo& c){ c.onMousePress(mp); }, posControl);
    }
    void onMouseRelease(sf::Vector2f mp, sf::Vector2f posControl) {
        distribuirEvento([&](Campo& c){ c.onMouseRelease(mp); }, posControl);
    }

private:
    int renglones() const {
        int r = 1;
        for(const auto& c : campos)
            if(c->nuevoRenglon) r++;
        return r;
    }

    // pasa el evento a todos los campos — cada campo hace su propio hit test
    template<typename Fn>
    void distribuirEvento(Fn fn, sf::Vector2f /*posControl*/) {
        for(auto& c : campos) fn(*c);
    }

    std::vector<std::unique_ptr<Campo>> campos;
    float altoFila;
};

} // namespace dsv
#endif