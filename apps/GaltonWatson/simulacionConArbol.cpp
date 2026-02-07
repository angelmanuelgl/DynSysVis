#include <Libreria.hpp>


#include <SFML/Graphics.hpp>


#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <fstream>
#include <string>




enum Sex { MALE, FEMALE };

struct Pulpo {
    int id;
    int gen;
    Sex sex;
    // padres
    int padre_id; 
    int madre_id;

    Pulpo(int _id, int _gen, Sex _s, int _f, int _m)
        : id(_id), gen(_gen), sex(_s), padre_id(_f), madre_id(_m ){}
};

int main( ){
    // --- cargar colores ---
    Tema::cargar("assets/config/colores.txt");

    // --- configurar ventana ---
    sf::RenderWindow window;
    Sistema::inicializarVentana(window, "Simulacion de Modelo Dalton Watson - Tesis");

    // ---  Interfaz ---
    

    // --- paneles ----
    Panel panel_m(window, Tema::c("azul"),  "Males / Machos M(t)",3,4);
    panel_m.positionAbsoluta(Ubicacion::ArribaIzq);

    Panel panel_f(window,  Tema::c("rosa"),  "Females / Hembras F(t)",3,4); 
    panel_f.positionRelativa(RelativoA::Abajo, panel_m);

    Panel panelCircular(window,  Tema::c("amarillo"),  "Poblacion", 3, 2); 
    panelCircular.positionAbsoluta(Ubicacion::AbajoIzq);
    // --- graficas ---

    auto* grafica_m = panel_m.crearContenido<GraficaTiempo>(Tema::c("azul"));
    auto* grafica_f = panel_f.crearContenido<GraficaTiempo>(Tema::c("rosa"));

    std::vector<sf::Color> colores = { Tema::c("azul"), Tema::c("rosa"), Tema::c("recolectoras") };
    auto circ = panelCircular.crearContenido<GraficoCircular>(  );
    circ->personalizarColores( colores );

    grafica_f -> configurarMaxPoints(10);
    grafica_m -> configurarMaxPoints(10);


    /// --- parametros --- 
    int max_generations = 100; // Cuidado: un numero alto genera archivos gigantes
    double lambda = 2.2;
    int next_id = 0;
    double probabilidadmale = 0.5f;
    int init_size = 40; 

    std::vector<Pulpo> todosLosIndividuos;
    std::random_device rd;
    std::mt19937 gen(rd());

    

    // --- simulacion ---
    int cnt_males=0, cnt_females=0;
    int generacionActual = 0;
    int generacionActualFin = init_size;
    int g = 0;
    bool seExtinguio = false;

    //  --- Poblacion Inicial --- 
    // 2 machos, 2 hembras
    for( int i = 0; i < init_size; i++){
        Sex s = (i % 2 == 0) ? MALE : FEMALE;
        if( s == MALE ) cnt_males++;
        else cnt_females++;
        // emplace_back enlugar de pus_back
        todosLosIndividuos.emplace_back(next_id++, 0, s, -1, -1);
    }
    
    
    grafica_m->addValue(cnt_males);
    grafica_f->addValue(cnt_females);
    
    // --- IMPORTANTE: control del tiempo --
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    sf::Time ups = sf::seconds(0.3f); // Update por segundo


    while( window.isOpen() ){ 
        sf::Event event;
        while( window.pollEvent(event) ){
            if( event.type == sf::Event::Closed) window.close();
        }

        // --- --- ---- simulacion --- --- ----
        accumulator += clock.restart();
        // cada paso es una nuvea generacion
        // while( si ya apso el timepo suficiente y no hemos lelgado al maximo de generaciones )
        while( accumulator >= ups &&  g < max_generations && !seExtinguio ){ 
            
    
            std::vector<int> males, females;
            
            //  --- separar machos y hembras--- 
            // la generacion actual esta justo en los indices [generacionActual, size ]
            for( int i = generacionActual; i < generacionActualFin; i++ ){
                if( todosLosIndividuos[i].sex == MALE) males.push_back(todosLosIndividuos[i].id);
                else females.push_back(todosLosIndividuos[i].id);
            }

            // --- agregar datos  ---
            // solo los de esta generacion deben ser agregados
            grafica_m->addValue(females.size());
            grafica_f->addValue(males.size());
            
            circ -> addValues( {females.size(), males.size()} );

            std::cout <<  "generacion " <<  g << " (m,f) = " << males.size() << ' ' << females.size() << '\n';
            
            
            // --- simular parejas para las sigueite --- 


            //TODO: min, max, f, m, otros ejemplos
            int num_couples = std::min(males.size(), females.size());
            if( num_couples == 0){
                std::cout << "extincion\n";
                seExtinguio = true;
                
                grafica_m -> addValue(0);
                grafica_f -> addValue(0);
                grafica_f -> addValue(0);


                break;
            } 

            int generacionNext = todosLosIndividuos.size();

            // --- reproduccion ---
            std::poisson_distribution<int> poisson(lambda); // hijos
            std::bernoulli_distribution bernoulli( probabilidadmale ); // macho o empra
            
            
            // todo
            // elegir unforee la primera herba // esta eloige unif macho U(n)
            // elegir unirforme la segunda 
            // 
            // todo fijar hebras // unforme sobre mahocs // U(n) U(n-1) ... U(n-2) ... 

            std::uniform_int_distribution queMale(0,  (int)males.size()-1 ); // macho o empra
            std::uniform_int_distribution queFemale(0,  (int)females.size()-1 ); // macho o empra

            for( int i = 0; i < num_couples; i++ ){
                int cant_hijos = poisson(gen);
                for( int j = 0; j < cant_hijos; j++ ){
                    Sex sex = bernoulli(gen) ? MALE : FEMALE; // true = male
                    // guardar hijo
                    int imale = queMale(gen);
                    int ifemale = queFemale(gen);
                    todosLosIndividuos.emplace_back(next_id++, g + 1, sex, males[imale], females[ifemale]);
                }
            }

            generacionActual = generacionNext;
            generacionActualFin = todosLosIndividuos.size();


          

            g++;
            accumulator = sf::Time::Zero; // restar el tiempo usado
        } // end while



        // --- --- ---- graficar --- --- ----
        window.clear(sf::Color(15, 15, 15));

        panel_m.draw();
        panel_f.draw();
        panelCircular.draw();

        window.display();
    }


    // //  --- --- ----  exportar  --- --- ----  
    // // a formato Graphviz (.dot)
    std::ofstream file("arbol_genealogico.dot");
    file << "digraph G {\n  rankdir=LR;\n  node [style=filled];\n";

    for( const auto& ind : todosLosIndividuos ){
        std::string color = (ind.sex == MALE) ? "lightblue" : "pink";
        file << "  " << ind.id << " [label=\"ID:" << ind.id << " (G" << ind.gen << ")\", fillcolor=" << color << "];\n";
        
        if( ind.padre_id != -1 ){
            file << "  " << ind.padre_id << " -> " << ind.id << " [color=blue];\n";
            file << "  " << ind.madre_id << " -> " << ind.id << " [color=red];\n";
        }
    }
    file << "}\n";
    file.close();

    std::cout << "Simulacion terminada\nSe registraron " << todosLosIndividuos.size() << " individuos en " << g<< " generaciones\n";
    std::cout << "Archivo 'arbol_genealogico.dot' generado.\n";

    return 0;
}