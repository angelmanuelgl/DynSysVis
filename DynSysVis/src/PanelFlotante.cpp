#include "dsv/menu/PanelFlotante.hpp"

namespace dsv {



PanelFlotante::PanelFlotante(
    sf::RenderWindow&  window,
    const std::string& etiqueta,
    HandleOrientacion  orientacion,
    DespliegueDir      despliegue,
    sf::Color          colorBorde)
    :
    window(window),
    orientacion(orientacion),
    despliegue(despliegue),
    handle(window, etiqueta, colorBorde),
    cuerpo(window, "",       colorBorde),
    etiquetaOriginal(etiqueta)
{
    // ── tamano del handle segun orientación ─────────────────────
    sf::Vector2f sizeH = (orientacion == HandleOrientacion::Horizontal)
        ? sf::Vector2f{ HANDLE_ANCHO_H, HANDLE_ALTO_H }
        : sf::Vector2f{ HANDLE_ANCHO_V, HANDLE_ALTO_V };

    handle.setSizeAbsoluto(sizeH);


    // ── cuerpo empieza con tamano default hasta que haya contenido
    cuerpo.setSizeAbsoluto({ 250.f, 350.f });
    recalcularPosCuerpo();


    // radio // espaciado panel // amrgen venata
    handle.configurarMedidas(radioHandle, 0.2f, 0.1f );
}



// ─────────────────────────────────────────────────────────────────
// ── HELPERS ──────────────────────────────────────────────────────
// ────────────────────────────────────────────────────────────────

bool PanelFlotante::mouseEnHandle(sf::Vector2f mousePos) const {
    sf::Vector2f pH = handle.getPosition();
    sf::Vector2f sH = handle.getSize();
    return mousePos.x >= pH.x && mousePos.x <= pH.x + sH.x
        && mousePos.y >= pH.y && mousePos.y <= pH.y + sH.y;
}

void PanelFlotante::recalcularTamanoCuerpo() {
    sf::Vector2f tam = (contenidoPtr) ? contenidoPtr->getSize() : sf::Vector2f(250.f, 350.f);
    cuerpo.setSizeAbsoluto(tam);
    recalcularPosCuerpo();  // posicion depende del tamaño del cuerpo
}

void PanelFlotante::recalcularPosCuerpo() {
    sf::Vector2f pH = handle.getPosition();
    sf::Vector2f sH = handle.getSize();
    sf::Vector2f sC = cuerpo.getSize();
    sf::Vector2f pos;

    switch(despliegue) {
        case DespliegueDir::Abajo:  pos = { pH.x, pH.y + sH.y + ESPACIADO }; break;
        case DespliegueDir::Arriba: pos = { pH.x, pH.y - sC.y - ESPACIADO }; break;
        case DespliegueDir::Der:    pos = { pH.x + sH.x + ESPACIADO, pH.y }; break;
        case DespliegueDir::Izq:    pos = { pH.x - sC.x - ESPACIADO, pH.y }; break;
        case DespliegueDir::AbajoIzq: 
            // Alinea el borde derecho del cuerpo con el del handle
            // pos.x = (X del handle + Ancho handle) - Ancho cuerpo
            pos = { (pH.x + sH.x) - sC.x, pH.y + sH.y + ESPACIADO }; 
            break;
        case DespliegueDir::ArribaIzq: 
            // Alinea el borde derecho del cuerpo con el del handle pero arriba
            pos = { (pH.x + sH.x) - sC.x, pH.y - sC.y - ESPACIADO }; 
            break;
    }
    cuerpo.setPosition(pos.x, pos.y);
}
void PanelFlotante::ajustarOrientacionHandle() {

    // por si lo encesitamos en vertical
    std::string resultado = "";
    for (size_t i = 0; i < etiquetaOriginal.length(); ++i) {
        resultado += etiquetaOriginal[i];
        if (i < etiquetaOriginal.length() - 1) {
            resultado += '\n'; // Insertamos un salto de línea tras cada carácter
        }
    }
    

    // ahora si
    bool esV = (orientacion == HandleOrientacion::Vertical);
    
    // 1. Determinar tamaño físico del handle
    sf::Vector2f sizeH(
        esV ? HANDLE_ANCHO_V : HANDLE_ANCHO_H,
        esV ? HANDLE_ALTO_V : HANDLE_ALTO_H
    );
    handle.setSizeAbsoluto(sizeH);

    // 2. Formatear el texto según la orientación
    // Asumimos que guardaste la 'etiqueta' original en un miembro de clase
    std::string textoFinal = esV ? resultado : etiquetaOriginal;
    
    handle.setTitulo(textoFinal);

    // 3. Ajustar posición del cuerpo por si el handle cambió de tamaño
    recalcularPosCuerpo();

}


// ─────────────────────────────────────────────────────────────────
// ── EVENTOS ──────────────────────────────────────────────────────
// ────────────────────────────────────────────────────────────────

// ── update ──────────────────────────────────────────────────────
void PanelFlotante::gestionarEvento(const sf::Event& evento ){
    sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePosI);
    
    bool click = (evento.type == sf::Event::MouseButtonPressed && 
                  evento.mouseButton.button == sf::Mouse::Left);

    //  Actualizar el estado del handle (hover y apertura)    
    update(mousePos, click);
    
    if( abierto && contenidoPtr ){
        // Simplemente delegamos el evento completo
        contenidoPtr->gestionarEvento(evento, window);
    }

    // // Pasar eventos al contenido (MenuFlotante) SOLO si está abierto
    // if (abierto && contenidoPtr) {
    //     // Obtenemos el origen del cuerpo para que el menú sepa dónde está dibujado
    //     sf::Vector2f origenCuerpo = cuerpo.getPosition();

    //     // Casteamos dinámicamente o asumimos que es un MenuFlotante
    //     // Si quieres ser genérico, podrías añadir estos métodos a la clase Objeto
    //     auto menu = dynamic_cast<MenuFlotante*>(contenidoPtr);
        
    //     if( menu){
    //         if (evento.type == sf::Event::MouseMoved) {
    //             menu->onMouseMove(mousePos, origenCuerpo);
    //         }
    //         else if (evento.type == sf::Event::MouseButtonPressed) {
    //             menu->onMousePress(mousePos, origenCuerpo);
    //         }
    //         else if (evento.type == sf::Event::MouseButtonReleased) {
    //             menu->onMouseRelease(mousePos, origenCuerpo);
    //         }
    //     }
    // }

    ajustarOrientacionHandle();
}

void PanelFlotante::update(sf::Vector2f mousePos, bool click) {

    // ── hover ────────────────────────────────────────────────────
    bool sobreHandle = mouseEnHandle(mousePos);
    
    if(sobreHandle != enHover) {
        enHover = sobreHandle;
    }

    // ── toggle al hacer click sobre el handle ────────────────────
    if(click && sobreHandle) {
        abierto = !abierto;

        DSV_LOG_INFO(abierto ? "PanelFlotante abierto" : "PanelFlotante cerrado");
    }

    // ── pasar eventos al contenido si esta abierto ───────────────
    if( abierto ) {
        // cuando MenuFlotante este listo recibira el mouse aqui
        // contenido->update(mousePos, click);
    }
}

// ── draw ────────────────────────────────────────────────────────

void PanelFlotante::draw() {

    // por si agregmaos bootnes o cosas entro del menu despeus de crear los obejtos
    recalcularTamanoCuerpo();
    recalcularPosCuerpo();

    // ── handle: siempre se dibuja ────────────────────────────────
     
    handle.setBorderWidth(enHover ? borde*2.0f :borde);

    handle.draw();



    // ── cuerpo: solo si esta abierto ─────────────────────────────
    if(abierto) {
        cuerpo.draw();
    }
}


// ─────────────────────────────────────────────────────────────────
// ── ESTIlo ──────────────────────────────────────────────────────
// ────────────────────────────────────────────────────────────────

void PanelFlotante::setColorFondo(sf::Color color){
    handle.setColorFondo(color);
    cuerpo.setColorFondo(color);
}    
void PanelFlotante::setDegradado(sf::Color colorTop, sf::Color colorBot){
    handle.setDegradado(colorTop, colorBot );
    cuerpo.setDegradado(colorTop, colorBot );
}    
void PanelFlotante::setBorderWidth(float grosor){
    handle.setBorderWidth(grosor);
    cuerpo.setBorderWidth(grosor);
}    

} // namespace dsv