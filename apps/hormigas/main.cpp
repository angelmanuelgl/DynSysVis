/*  * autor: Angel Manuel Gonzalez Lopez 
    * github: https://github.com/angelmanuelgl
    * web: https://angelmanuelgl.github.io/
    * 
    * proyecto: DynSysVis - - - - - - - - - - - - - - - - - - - 
 */
/*  MAIN.cpp
    ejemplo para usar mi libreria 
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>


#include "DynSysVis.hpp"

using namespace dsv;


struct AntColony_Model {
    static constexpr size_t dim       = 3;
    static constexpr size_t noise_dim = 0;

    // Parámetros del modelo (con tus valores por defecto)
    float bO      = 0.05f;
    float betaOG  = 4.0f;
    float betaGR  = 2.5f;
    float betaRG  = 1.2f;
    float kG      = 50.0f;
    float kR      = 50.0f;
    float dG      = 0.005f;
    float dR      = 0.005f;

    enum { O, G, R }; // Mapeo de índices

    void drift(const std::array<float, dim>& x,
               float /*t*/,
               std::array<float, dim>& out) const 
    {
        // x[O] = Obreras, x[G] = Guerreras, x[R] = Recolectoras
        
        // dO/dt = bO*O - (betaOG / (kG + G))*O
        out[O] = (bO * x[O]) - (betaOG / (kG + x[G])) * x[O];

        // dR/dt = (betaGR / (kR + R)) * G - (betaRG / (kG + G)) * R - (dR * R)
        out[R] = (betaGR / (kR + x[R])) * x[G] 
                 - (betaRG / (kG + x[G])) * x[R] 
                 - (dR * x[R]);
                 
        // dG/dt = (betaOG / (kG + G)) * O - (betaGR / (kR + R)) * G - (dG * G) + (betaRG / (kG + G)) * R
        out[G] = (betaOG / (kG + x[G])) * x[O] 
                 - (betaGR / (kR + x[R])) * x[G] 
                 - (dG * x[G]) 
                 + (betaRG / (kG + x[G])) * x[R];

        
    }
};


int main( ){
    // --- cargar colores --- DSV
    dsv::Color::cargar("assets/config/colores.txt");

    // --- configurar ventana --- SFML
    sf::RenderWindow window;
    dsv::Sistema::inicializarVentana(window, "DynSysVis RT - Colonia de Hormigas");



    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    // --- --- --- --- --- --- MODELO --- --- --- --- --- --- ---
    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---

    // --- INSTANCIA DEL MODELO ---
    // using Ants = dsv::mod::AntColony_Model;
    using Ants = AntColony_Model;
    dsv::mod::Instance<Ants> hormigas;
    
    // Condiciones iniciales
    hormigas.state[Ants::O] = 60.0f;
    hormigas.state[Ants::G] = 20.0f;
    hormigas.state[Ants::R] = 10.0f;

    // Referencia corta al modelo para los menús
    auto& m = hormigas.getModel();

    // --- MENU PARA AJUSTAR MODELO (VINCULADO A LA INSTANCIA) ---
    // PANEL
    dsv::PanelFlotante PanelMenuHormigas(
        window, 
        "info", 
        dsv::HandleOrientacion::Vertical,
        dsv::DespliegueDir::Der,
        dsv::Color::morado      
    );
    PanelMenuHormigas.setDegradado( sf::Color(50,30,60,240),  sf::Color(20,20,25, 240) ); 
    PanelMenuHormigas.positionAbsoluta( dsv::Ubicacion::CentroIzq );
    
    
    // MENU
    MenuFlotante* menuHormigas = PanelMenuHormigas.crearContenido<MenuFlotante>(6.f, 12.f);

    // --- contenido --- 
    // obreras
    CampoControl& mH_fila1 = menuHormigas->agregarFila();
    mH_fila1.agregar<dsv::CampoBarra>("Obreras ", &hormigas.state[Ants::O], 0.f, 500.f, sf::Color(80,240,100));
    
    CampoControl& mH_fila2 = menuHormigas->agregarFila();
    mH_fila2.agregar<dsv::CampoDeslizador>("Nacimiento Obreras ", &m.bO, 0.0f, 1.0f);

    // Recolectoras
    CampoControl& mH_fila3 = menuHormigas->agregarFila();
    mH_fila3.agregar<dsv::CampoBarra>("Recolectoras ", &hormigas.state[Ants::R], 0.f, 500.f, dsv::Color::oro);
    
    CampoControl& mH_fila4 = menuHormigas->agregarFila();
    mH_fila4.agregar<dsv::CampoDeslizador>("Muerte Recolectoras ", &m.dR, 0.0f, 1.0f);

    // Guerreras
    CampoControl& mH_fila5 = menuHormigas->agregarFila();
    mH_fila5.agregar<dsv::CampoBarra>("Guerreras ", &hormigas.state[Ants::G], 0.f, 500.f, dsv::Color::rojo);
    
    CampoControl& mH_fila6 = menuHormigas->agregarFila();
    mH_fila6.agregar<dsv::CampoDeslizador>("Muerte Guerreras ", &m.dG, 0.0f, 1.0f);


   
    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    // --- --- --- --- --- --- PANELES --- --- --- --- --- --- --
    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    
    // --- tablero con datos --- DSV
    dsv::Layout miLayout = {
        "fa1 .   . o o o",
        "fa2 fa3 . r r r",
        "cir cir . g g g",
        "cir cir all all t t"
    };
    dsv::Tablero tablero(window, miLayout);

    /// GraficaTiempo
    Vista<GraficaTiempo> obreras      = tablero.add<dsv::GraficaTiempo>("obreras", dsv::Color::verde, "o", dsv::Color::verde);
    Vista<GraficaTiempo> guerreras    = tablero.add<dsv::GraficaTiempo>("guerreras", dsv::Color::rojo, "g", dsv::Color::rojo);
    Vista<GraficaTiempo> recolectoras = tablero.add<dsv::GraficaTiempo>("recolectoras", dsv::Color::oro, "r", dsv::Color::oro);
    
    Vista<GraficaTiempo> total = tablero.add<dsv::GraficaTiempo>("Poblaciones Totalde la totaldiad toal de hormigas totales e", dsv::Color::celeste, "t" );

    /// EspacioFase2D
    Vista<EspacioFase2D> OG = tablero.add<dsv::EspacioFase2D>("Espacio Fase (O, G)", dsv::Color::azul, "fa1", dsv::Color::azul );
    Vista<EspacioFase2D> OR = tablero.add<dsv::EspacioFase2D>("Espacio Fase (O, R)", dsv::Color::aqua, "fa2",   dsv::Color::aqua );
    Vista<EspacioFase2D> RG = tablero.add<dsv::EspacioFase2D>("Espacio Fase (R, G)", dsv::Color::cian, "fa3",   dsv::Color::cian );

    /// GraficoCircular
    Vista<GraficoCircular> pie = tablero.add<dsv::GraficoCircular>("Poblacion de Hormigas", dsv::Color::aqua, "cir");

    pie -> personalizarColores( { dsv::Color::verde ,  dsv::Color::oro , dsv::Color::rojo} );

    /// GraficaTiempo // tambien admite multiserie
    Vista<GraficaTiempo> triple = tablero.add<dsv::GraficaTiempo>("Poblaciones de Hormigas", dsv::Color::celeste, "all" );

    triple -> agregarSerie("recolectoras", dsv::Color::oro);
    triple -> agregarSerie("obreras"     , dsv::Color::verde);
    triple -> agregarSerie("guerreras"   , dsv::Color::rojo);

    obreras.objeto.configurarMaxLim(40); // 40s
    guerreras.objeto.configurarMaxLim(40); // 40s
    recolectoras.objeto.configurarMaxLim(40); // 40s
    triple.objeto.configurarMaxLim(40); // 40s


    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    // --- --- --- --- --- --- MENU DE TIEMPO --- --- --- --- ---
    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    // eventos
    bool pausa = true;           
    float timeScale = 1.0f;       
    // fps e info
    sf::Clock fpsClock;        
    long long frameCount = 0;
    float fps = 0;
    float tiempo=0.0f;
    
    // PANEL
    sf::Color colorMENU = dsv::Color::naranja;
    sf::Color colorMenu = dsv::Color::oro;
    dsv::PanelFlotante PanelMenuTiempo(
        window, 
        "Menu Tiempo", 
        dsv::HandleOrientacion::Horizontal,
        dsv::DespliegueDir::Abajo,
        colorMENU          
    );
    PanelMenuTiempo.setDegradado( sf::Color(60,60,40,240),   sf::Color(40,40,30,220) ); 
    PanelMenuTiempo.positionAbsoluta( dsv::Ubicacion::ArribaCentro );
    

    // MENU
    MenuFlotante* menu = PanelMenuTiempo.crearContenido<MenuFlotante>(6.f, 12.f);

    // COTENIDO
    CampoControl& menu_fila1 = menu->agregarFila();
    menu_fila1.agregar<dsv::CampoTexto>(" - - - - - CONTROLADOR - - - - - ");

    CampoControl& menu_fila2 = menu->agregarFila();
    menu_fila2.agregar<dsv::CampoVariable>("fps",      &fps);
    menu_fila2.agregar<dsv::CampoVariable>("tiempo",   &tiempo);

    CampoControl& menu_fila3 = menu->agregarFila();
    menu_fila3.agregar<dsv::CampoBoton>("<<", [&]{ timeScale = std::max(0.1f, timeScale - 0.1f); },  colorMENU);
    menu_fila3.agregar<dsv::CampoToggleTexto>("", &pausa, "Pausa", "Play", colorMENU);
    menu_fila3.agregar<dsv::CampoBoton>(">>", [&]{ timeScale += 0.1f; }, colorMENU);

    CampoControl& menu_fila4 = menu->agregarFila();
    menu_fila4.agregar<dsv::CampoVariable>("Velocidad:  x",    &timeScale);

    CampoControl& menu_fila5 = menu->agregarFila();
    menu_fila5.agregar<dsv::CampoBoton>("Reset", [&]{  });
    menu_fila5.agregar<dsv::CampoToggle>("Pausa", &pausa,  colorMENU);



    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---
    // --- --- --- --- --- --- SIMULACION --- --- --- --- --- ---
    // --- --- --- --- --- --- --- -- --- --- --- --- --- --- ---

    // --- --- --- CONTROL TIEMPO --- --- ---
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.005f); // Update por segundo
    
   
    while( window.isOpen() ){
        // --- --- --- ---  ECVENTOS --- --- --- --- 
        sf::Event event;
        while( window.pollEvent(event) ){
            if( event.type == sf::Event::Closed) window.close();

            PanelMenuTiempo.gestionarEvento(event);
            PanelMenuHormigas.gestionarEvento(event);


            // extra
            if( event.type == sf::Event::KeyPressed ){ 
                if( event.key.code == sf::Keyboard::Space ){ 
                    pausa = !pausa; 
                }
                if( event.key.code == sf::Keyboard::Up ){ 
                    timeScale += 0.2f; // Acelerar
                }
                if( event.key.code == sf::Keyboard::Down ){ 
                    timeScale = std::max(0.1f, timeScale - 0.2f); // Desacelerar (mínimo 0.1)
                }
            }
        }

        // --- --- --- ---  SOMULACION --- --- --- --- 
        sf::Time elapsed = clock.restart();

        //  Si no está pausado, lo sumamos al acumulador afectado por la escala
        if( !pausa  ){ 
            accumulator += elapsed * timeScale;
        }

        while( accumulator >= ups  ){
            // --- --- --- --- ACTUALIZAR MODELO --- --- --- ---
            float dt = ups.asSeconds();
            
            // ¡Aquí usamos el integrador oficial de la librería!
            // Puedes cambiar a sim::E_step si prefieres Euler, pero RK4 es más estable.
            dsv::sim::step(hormigas, dt);

            tiempo = hormigas.t;  // muestra
            accumulator -= ups;
        }

        // --- --- agregar datos a graficas --- --- 
        if( !pausa ){
            float O_val = hormigas.state[Ants::O];
            float G_val = hormigas.state[Ants::G];
            float R_val = hormigas.state[Ants::R];
            float t_val = hormigas.t;

            obreras->push_back(O_val, t_val);
            guerreras->push_back(G_val, t_val);
            recolectoras->push_back(R_val, t_val);
            total->push_back(O_val + G_val + R_val, t_val);

            triple->push_back(O_val, t_val, "obreras");
            triple->push_back(G_val, t_val, "guerreras");
            triple->push_back(R_val, t_val, "recolectoras");

            OG->push_back(O_val, G_val);
            OR->push_back(O_val, R_val);
            RG->push_back(R_val, G_val);

            pie->push_back({O_val, G_val, R_val});
        }

        // --- --- --- --- --- calcular FPS --- --- --- --- --- 
        frameCount++;
        if( fpsClock.getElapsedTime().asSeconds() >= 1.0f ){ 
            fps = frameCount;
            frameCount = 0;
            fpsClock.restart();
        }


        // --- --- --- ---  Renderizado --- --- --- --- 
        // paneles de layout
        tablero.draw();
        // paneles flotantes
        PanelMenuTiempo.draw();
        PanelMenuHormigas.draw();
        
        window.display();
    }

    return 0;
}