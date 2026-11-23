/*
 * proyecto: DynSysVis RT -- SIR Binomial (cadena estocastica en tiempo discreto)
 *
 * Modelo: S_{n+1} = S_n - e1,  I_{n+1} = I_n - e2 + e1,  R_{n+1} = R_n + e2
 *   e1 ~ Binomial(S_n, q_n)   e2 ~ Binomial(I_n, p_n)
 *
 * Cada paso de simulacion = 1 dia (n es entero). Las 10 instancias comparten
 * el mismo modelo (vincular) y difieren solo por el ruido binomial.
 *
 * Layout:
 *   +--------------+--------------+
 *   |  3D (S,I,R)  |  all S,I,R   |
 *   |  10 MC       |  instancia 0 |
 *   +--------------+--------------+
 *   |  I(t) 10 MC  | fase S vs I  | pie |
 *   +--------------+--------------+-----+
 */

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "DynSysVis.hpp"

using SIRB = dsv::mod::SIR_Binomial_Model<>;   // Type = int por defecto
using Inst = dsv::mod::Instance<SIRB>;

static constexpr int NUM_SIM = 10;
static constexpr int N_TOTAL = 1000;
static constexpr int S0 = 990;
static constexpr int I0 = 10;
static constexpr int R0_INI = 0;


int main() {

    // --- Init -----------------------------------------------------------------
    dsv::Color::cargar("assets/config/colores.txt");
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT -- SIR Binomial");


    // =========================================================================
    //  MODELO BASE Y 10 INSTANCIAS MONTE CARLO
    // =========================================================================
    SIRB modeloBase;

    std::vector<Inst> sims(NUM_SIM);
    auto initSims = [&]() {
        for (auto& s : sims) {
            s.state[SIRB::S] = S0;
            s.state[SIRB::I] = I0;
            s.state[SIRB::R] = R0_INI;
            s.t = 0;
            s.model_ref.vincular(modeloBase);
        }
    };
    initSims();

    // Bridges float para la UI: la libreria pide float*, el modelo usa double/int
    float S_f = 0.f, I_f = 0.f, R_f = 0.f;
    float tiempo = 0.f;

    float b0_f = static_cast<float>(modeloBase.beta_0);
    float a_f  = static_cast<float>(modeloBase.alpha);
    float per_f = static_cast<float>(modeloBase.periodo);
    float g0_f = static_cast<float>(modeloBase.gamma_0);
    float d_f  = static_cast<float>(modeloBase.delta);
    float tau_f = static_cast<float>(modeloBase.tau);

    float pico_I = 0.f;     // maximo de I observado (instancia 0)
    float dia_pico = 0.f;   // dia en que ocurre
    float R0_aprox = 0.f;

    std::vector<sf::Color> paleta(NUM_SIM);
    for (int i = 0; i < NUM_SIM; ++i)
        paleta[i] = dsv::Color::Oceano(i, NUM_SIM);


    // =========================================================================
    //  LAYOUT Y TABLERO
    // =========================================================================
    dsv::Layout miLayout = {
        "3d  3d  all all",
        "3d  3d  all all",
        "ti  ti  fSI pie",
    };
    dsv::Tablero tablero(window, miLayout,
                         dsv::Color::panelUp, dsv::Color::panelDown);


    // --- Grafica3D: trayectorias (S, I, R) ----------------------------------
    dsv::Vista<dsv::Grafica3D> fase3D =
        tablero.add<dsv::Grafica3D>("Atractor SIR (S, I, R) -- 10 MC",
                                    dsv::Color::cian, "3d");
    fase3D.objeto.getEjes().setLimites(0.0f, static_cast<float>(N_TOTAL));
    {
        auto& g = fase3D.objeto.getGestor();
        g.setMaxPointsSeries(400);
        g.setGrosorSeries(1.5f);
        g.setDifuminadoSeries(true);
        for (int i = 0; i < NUM_SIM; ++i)
            g.agregarSerie("MC" + std::to_string(i), paleta[i]);
    }


    // --- GraficaTiempo multiserie: S, I, R de la instancia 0 -----------------
    dsv::Vista<dsv::GraficaTiempo> grafAll =
        tablero.add<dsv::GraficaTiempo>("S(t), I(t), R(t) -- instancia 0",
                                        dsv::Color::celeste, "all");
    grafAll->agregarSerie("S", dsv::Color::celeste);
    grafAll->agregarSerie("I", dsv::Color::rojo);
    grafAll->agregarSerie("R", dsv::Color::verde);
    grafAll.objeto.configurarVentanaTiempo(300.0f);
    grafAll.objeto.configurarLimitesY(0.0f, static_cast<float>(N_TOTAL));
    grafAll.objeto.ponerSombreado(false);


    // --- GraficaTiempo: I(t) de las 10 instancias (banda Monte Carlo) --------
    dsv::Vista<dsv::GraficaTiempo> grafI =
        tablero.add<dsv::GraficaTiempo>("I(t) -- 10 trayectorias MC",
                                        dsv::Color::rojo, "ti");
    for (int i = 0; i < NUM_SIM; ++i)
        grafI->agregarSerie("I" + std::to_string(i), paleta[i]);
    grafI.objeto.configurarVentanaTiempo(300.0f);
    grafI.objeto.configurarLimitesY(0.0f, static_cast<float>(N_TOTAL));
    grafI.objeto.ponerSombreado(false);


    // --- EspacioFase2D: S vs I -----------------------------------------------
    dsv::Vista<dsv::EspacioFase2D> faseSI =
        tablero.add<dsv::EspacioFase2D>("Fase S vs I -- 10 MC",
                                        dsv::Color::cian, "fSI");
    faseSI->activarSeguimiento(false);
    faseSI->activarAutoescalado(true);
    faseSI->ponerDesvanecido(false);
    faseSI->ponerCabeza(true);
    faseSI->configurarMaxPoints(2000);
    for (int i = 0; i < NUM_SIM; ++i)
        faseSI->agregarSerie("SI" + std::to_string(i), paleta[i]);


    // --- GraficoCircular: distribucion S, I, R de la instancia 0 -------------
    dsv::Vista<dsv::GraficoCircular> pie =
        tablero.add<dsv::GraficoCircular>("Distribucion [S:I:R]",
                                          dsv::Color::aqua, "pie");
    pie->personalizarColores({dsv::Color::celeste, dsv::Color::rojo, dsv::Color::verde});

    tablero.setPanelDegradado(dsv::Color::panelUp, dsv::Color::panelDown);


    // =========================================================================
    //  PANEL PARAMETROS (izquierda)
    // =========================================================================
    dsv::PanelFlotante PanelParams(
        window, "Parametros",
        dsv::HandleOrientacion::Vertical,
        dsv::DespliegueDir::Der,
        dsv::Color::rojo
    );
    PanelParams.setDegradado(sf::Color(45, 8, 8, 245), sf::Color(22, 4, 4, 245));
    PanelParams.positionAbsoluta(dsv::Ubicacion::CentroIzq);

    dsv::MenuFlotante* menuP = PanelParams.crearContenido<dsv::MenuFlotante>(6.f, 14.f);

    menuP->agregarFila().agregar<dsv::CampoTexto>("-- Transmision --");
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("beta_0  Base",    &b0_f,  0.0f, 1.0f);
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("alpha  Estacion.", &a_f,  0.0f, 0.99f);
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("periodo (dias)", &per_f, 30.0f, 730.0f);

    menuP->agregarFila().agregar<dsv::CampoTexto>("-- Recuperacion --");
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("gamma_0  Base",  &g0_f,  0.001f, 0.5f);
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("delta  Mejora",  &d_f,   0.0f, 2.0f);
    menuP->agregarFila().agregar<dsv::CampoDeslizador>("tau  Adaptacion",&tau_f, 1.0f, 300.0f);

    menuP->agregarFila().agregar<dsv::CampoBoton>("Reset Params", [&]{
        modeloBase.beta_0  = 0.32;  modeloBase.alpha = 0.15;
        modeloBase.periodo = 365.0; modeloBase.gamma_0 = 0.08;
        modeloBase.delta   = 0.50;  modeloBase.tau   = 60.0;
        b0_f = 0.32f; a_f = 0.15f; per_f = 365.f;
        g0_f = 0.08f; d_f = 0.50f; tau_f = 60.f;
    }, dsv::Color::rojo);


    // =========================================================================
    //  PANEL ESTADO (derecha) -- instancia 0
    // =========================================================================
    dsv::PanelFlotante PanelEstado(
        window, "Estado",
        dsv::HandleOrientacion::Vertical,
        dsv::DespliegueDir::Izq,
        dsv::Color::cian
    );
    PanelEstado.setDegradado(sf::Color(5, 20, 35, 245), sf::Color(3, 10, 18, 245));
    PanelEstado.positionAbsoluta(dsv::Ubicacion::CentroDer);

    dsv::MenuFlotante* menuE = PanelEstado.crearContenido<dsv::MenuFlotante>(6.f, 14.f);

    menuE->agregarFila().agregar<dsv::CampoTexto>("-- Compartimentos (inst. 0) --");
    menuE->agregarFila().agregar<dsv::CampoBarra>("S", &S_f, 0.f, (float)N_TOTAL, dsv::Color::celeste);
    menuE->agregarFila().agregar<dsv::CampoBarra>("I", &I_f, 0.f, (float)N_TOTAL, dsv::Color::rojo);
    menuE->agregarFila().agregar<dsv::CampoBarra>("R", &R_f, 0.f, (float)N_TOTAL, dsv::Color::verde);

    menuE->agregarFila().agregar<dsv::CampoTexto>("-- Metricas --");
    menuE->agregarFila().agregar<dsv::CampoVariable>("Pico I =",   &pico_I);
    menuE->agregarFila().agregar<dsv::CampoVariable>("Dia pico =", &dia_pico);
    menuE->agregarFila().agregar<dsv::CampoVariable>("R0 aprox =", &R0_aprox);

    menuE->agregarFila().agregar<dsv::CampoBoton>("Reset C.I.", [&]{
        initSims();
        tiempo = 0.f; pico_I = 0.f; dia_pico = 0.f;
    }, dsv::Color::verde);


    // =========================================================================
    //  PANEL TIEMPO (arriba centro)
    // =========================================================================
    bool  pausa     = true;
    float timeScale = 1.0f;
    float fps = 0.0f;
    sf::Clock fpsClock;
    long long frameCount = 0;

    const sf::Color colorMENU = dsv::Color::naranja;

    dsv::PanelFlotante PanelTiempo(
        window, "Menu Tiempo",
        dsv::HandleOrientacion::Horizontal,
        dsv::DespliegueDir::Abajo,
        colorMENU
    );
    PanelTiempo.setDegradado(dsv::Color::naranja_dd % 230, dsv::Color::naranja_dd % 230);
    PanelTiempo.positionAbsoluta(dsv::Ubicacion::ArribaCentro);

    dsv::MenuFlotante* menuT = PanelTiempo.crearContenido<dsv::MenuFlotante>(6.f, 12.f);
    menuT->agregarFila().agregar<dsv::CampoTexto>("-- SIR Binomial (tiempo discreto) --");

    dsv::CampoControl& mt2 = menuT->agregarFila();
    mt2.agregar<dsv::CampoVariable>("fps", &fps);
    mt2.agregar<dsv::CampoVariable>("dia =", &tiempo);

    dsv::CampoControl& mt3 = menuT->agregarFila();
    mt3.agregar<dsv::CampoBoton>("<<", [&]{ timeScale = std::max(0.1f, timeScale - 0.1f); }, colorMENU);
    mt3.agregar<dsv::CampoToggleTexto>("", &pausa, "Pausa", "Play", colorMENU);
    mt3.agregar<dsv::CampoBoton>(">>", [&]{ timeScale += 0.1f; }, colorMENU);

    dsv::CampoControl& mt4 = menuT->agregarFila();
    mt4.agregar<dsv::CampoVariable>("Speed x", &timeScale);


    // =========================================================================
    //  BUCLE PRINCIPAL
    // =========================================================================
    sf::Clock clock;
    sf::Time  accumulator = sf::Time::Zero;
    // Cada "tick" del acumulador = 1 paso discreto (1 dia).
    sf::Time  ups = sf::seconds(0.3f);

    while (window.isOpen()) {

        // --- Eventos ----------------------------------------------------------
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            PanelTiempo.gestionarEvento(event);
            PanelParams.gestionarEvento(event);
            PanelEstado.gestionarEvento(event);
            fase3D->gestionarEvento(event, window);

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) pausa = !pausa;
                if (event.key.code == sf::Keyboard::Up)    timeScale += 0.2f;
                if (event.key.code == sf::Keyboard::Down)  timeScale = std::max(0.1f, timeScale - 0.2f);
                if (event.key.code == sf::Keyboard::R) {
                    initSims();
                    tiempo = 0.f; pico_I = 0.f; dia_pico = 0.f;
                }
            }
        }

        // --- Sincronizar UI -> modelo compartido (una vez por frame) ----------
        modeloBase.beta_0  = b0_f;
        modeloBase.alpha   = a_f;
        modeloBase.periodo = per_f;
        modeloBase.gamma_0 = g0_f;
        modeloBase.delta   = d_f;
        modeloBase.tau     = tau_f;

        // R0 aproximado: beta_0 / gamma_0 (valor base, sin estacionalidad)
        R0_aprox = (modeloBase.gamma_0 > 0.0)
                 ? static_cast<float>(modeloBase.beta_0 / modeloBase.gamma_0)
                 : 0.f;

        // --- Simulacion ------------------------------------------------------
        sf::Time elapsed = clock.restart();
        if (!pausa)
            accumulator += elapsed * timeScale;

        while (accumulator >= ups) {

            // Un paso discreto para todas las instancias.
            // step(sys) no recibe dt: el modelo es discreto (noise_dim=2).
            for (auto& s : sims)
                dsv::sim::step(s);

            // Registrar datos de este paso (dentro del while: no se pierden pasos)
            for (int i = 0; i < NUM_SIM; ++i) {
                const std::string si = std::to_string(i);
                const float S = static_cast<float>(sims[i].state[SIRB::S]);
                const float I = static_cast<float>(sims[i].state[SIRB::I]);
                const float R = static_cast<float>(sims[i].state[SIRB::R]);
                const float t = static_cast<float>(sims[i].t);

                fase3D.objeto.getGestor().push_back({S, I, R}, "MC" + si);
                faseSI->push_back(S, I, "SI" + si);
                grafI->push_back(I, t, "I" + si);

                if (i == 0) {
                    grafAll->push_back(S, t, "S");
                    grafAll->push_back(I, t, "I");
                    grafAll->push_back(R, t, "R");
                    pie->push_back({S, I, R});

                    S_f = S; I_f = I; R_f = R;
                    tiempo = t;
                    if (I > pico_I) { pico_I = I; dia_pico = t; }
                }
            }

            accumulator -= ups;
        }

        // Si no hubo pasos este frame, refrescar la barra de estado igualmente
        S_f = static_cast<float>(sims[0].state[SIRB::S]);
        I_f = static_cast<float>(sims[0].state[SIRB::I]);
        R_f = static_cast<float>(sims[0].state[SIRB::R]);

        // --- FPS -------------------------------------------------------------
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = static_cast<float>(frameCount);
            frameCount = 0;
            fpsClock.restart();
        }

        // --- Renderizado -----------------------------------------------------
        tablero.draw();
        PanelTiempo.draw();
        PanelParams.draw();
        PanelEstado.draw();
        window.display();
    }

    return 0;
}