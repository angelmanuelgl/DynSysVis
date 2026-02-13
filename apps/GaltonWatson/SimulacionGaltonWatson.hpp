#include <iostream>
#include <vector>
#include <random>
#include <bitset>
#include <numeric>
#include <fstream>

typedef int64_t ll;

#define MAXFAM 200

enum Sex { MALE, FEMALE };

struct Pulpo {
    int id;
    int gen;
    Sex sex;
    // padres
    int padre_id; 
    int madre_id;
    // 
    bool familia[MAXFAM];
    bool marca = false;
    bool resultadoInsesto = false;

    Pulpo(int _id, int _gen, Sex _s, int _f, int _m)
        : id(_id), gen(_gen), sex(_s), padre_id(_f), madre_id(_m ){

            for(int i=0; i<MAXFAM; i++ ) familia[i] = false;
        }

    // true si hay autointereccion en los arboles
    bool heredar( Pulpo pa, Pulpo ma ){
        bool ans = false;
        for(int i=0; i<MAXFAM; i++ ){
            if( pa.familia[i] &&  ma.familia[i]  ) ans = true;
            this->familia[i] =  (pa.familia[i] ||  ma.familia[i]);
        }
        if( ans ){
            marca = true;
            resultadoInsesto = true;
        } 
        return ans;
    }
    
};


/*
    --- --- ----  exportar  --- --- ----  
    formato Graphviz (.dot)
    https://edotor.net/
*/
void guardarGraphviz( std::vector<Pulpo>& todosLosIndividuos, int g  ){
    
    // marcar ancestros de individuos con incesto
    // atras hacia adelante para asegurar que la marca se propague
    for (int i = todosLosIndividuos.size() - 1; i >= 0; i--) {
        if (todosLosIndividuos[i].marca) {
            int p_id = todosLosIndividuos[i].padre_id;
            int m_id = todosLosIndividuos[i].madre_id;
            
            if (p_id != -1) {
                todosLosIndividuos[p_id].marca = true;
            }
            if (m_id != -1) {
                todosLosIndividuos[m_id].marca = true;
            }
        }
    }

    // guardar
    std::ofstream file("arbol_genealogico.dot");
    file << "digraph G {\n  rankdir=LR;\n  node [style=filled];\n";

    for( const auto& ind : todosLosIndividuos ){
        std::string color = (ind.sex == MALE) ? "lightblue" : "pink";
        if( ind.resultadoInsesto ) color = (ind.sex == MALE) ? "blue" : "red";

        file << "  " << ind.id << " [label=\" (" << ind.padre_id << ", " << ind.madre_id << ") ->" << ind.id << "\", fillcolor=" << color << "];\n";
        
        if( ind.padre_id != -1 ){
            std::string extra = ind.marca ? ", penwidth=3.0" : "";
            file << "  " << ind.padre_id << " -> " << ind.id << " [color=blue" << extra << "];\n";
            file << "  " << ind.madre_id << " -> " << ind.id << " [color=red" << extra << "];\n";
        }
    }
    file << "}\n";
    file.close();
    
    std::cout << "Archivo 'arbol_genealogico.dot' generado.\n";
}



// Función que ejecuta UNA simulación y devuelve la generación del primer incesto
// Si se extinguen antes de que ocurra, devolvemos un valor especial (ej. -1 o max_gen)
int ejecutarSimulacion(int size_inicial, double lambda, double probabilidadmale, int max_generations, bool guardar = false) {
    
    double probabilidadfemale = 1 - probabilidadmale;
    double lambda_m = lambda * probabilidadmale;
    double lambda_f = lambda * probabilidadfemale;

    std::vector<Pulpo> todosLosIndividuos;
    std::random_device rd;
    std::mt19937 gen(rd());


     // --- simulacion ---
    int next_id = 0;
    int cnt_males=0, cnt_females=0;
    int generacionActual = 0;
    int generacionActualFin = size_inicial;
    int g = 0;
    bool seExtinguio = false;
    bool insesto = false;

    //  --- Poblacion Inicial --- 
    // mitad machos, mitad hembras
    int familia = 1;
    for( int i = 0; i < size_inicial; i++){
        Sex s = (i % 2 == 0) ? MALE : FEMALE;
        if( s == MALE ) cnt_males++;
        else cnt_females++;
        // emplace_back enlugar de pus_back
        Pulpo p(next_id++, 0, s, -1, -1 );
        p.familia[ familia ] = true;

        todosLosIndividuos.push_back(p);
        
        // 0 1 // 2 3 // 4 5 //
        familia++; 

    }
    
    
    while(  g < max_generations && !seExtinguio && !insesto ){ 
        
        // --- --- --- --- --- --- --- --- --- --- //
        // --- --- VER GENERACION ACTUAL  --- --- //
        // --- --- --- --- --- --- --- --- --- --- //


        // --- guardar id de machos y hembras --- 
        // la generacion actual esta justo en los indices [generacionActual, generacionActualFin ]
        std::vector<int> id_males, id_females;
        for( int i = generacionActual; i < generacionActualFin; i++ ){
            if( todosLosIndividuos[i].sex == MALE) 
                id_males.push_back( todosLosIndividuos[i].id );
            else 
                id_females.push_back( todosLosIndividuos[i].id );
        }

        //! --- agregar datos de esta generacion ---
        // std::cout <<  "generacion " <<  g << " (m,f) = " << id_males.size() << ' ' << id_females.size() << '\n';
        
        
        // --- --- --- --- ---- ---- --- --- --- //
        // --- SIMULAR SIGUIENTE GENERACION  --- //
        // --- --- --- --- ---- ---- --- --- --- // 
        
        // con este indice iniciara la siguiente generacion
        int generacionNext = todosLosIndividuos.size();

        
        
        // --- --- --- ---  --- -- --- --- --- --- --- ---  //
        // --- --- --- ---  FORMAR PAREJAS --- --- --- ---  //
        // --- --- --- ---  --- -- --- --- --- --- --- ---  //

        int cantidadParejas = std::min( id_males.size(), id_females.size()) ; // TODO: min, max, f, m, otros ejemplos

        cantidadParejas = id_females.size() * std::min( 1, (int) id_males.size());
        // 10 1
        // 
        if( cantidadParejas == 0){
            std::cout << "extincion\n";
            seExtinguio = true;
            break;
        } 

        
        std::vector< std::pair<int,int> > parejas;
        std::vector<int> disponibles_males = id_males;
        std::vector<int> disponibles_females = id_females;

        // TODO: optimizar con shuffle y luego tomar los primeros n asi es O(n) // esto es O(n^2) por el erase \
        // TODO: investigar como funciona el shuffle 
        for( int i=0; i< cantidadParejas; i++ ){
            //  --- elegir uniforme un macho y una hembra  ---
            int cnt_disponibles_males =  disponibles_males.size() -1;
            std::uniform_int_distribution um(0, cnt_disponibles_males);

            int cnt_disponibles_females = (int)disponibles_females.size() -1;
            std::uniform_int_distribution uf(0, cnt_disponibles_females); 

            // indices en el arreglo
            int indice_f = uf(gen);
            int indice_m = um(gen);
            
            // id de los individuos
            int id_famale_elegido = disponibles_females[indice_f];
            int id_male_elegido = disponibles_males[indice_m];

            // --- guardar pareja  ---
            parejas.push_back({ id_male_elegido, id_famale_elegido }); 

            //  --- eliminar de la lista de disponibles  ---
            // disponibles_males.erase( disponibles_males.begin() + indice_m );
            disponibles_females.erase( disponibles_females.begin() + indice_f );
        }

        
        // --- --- --- --- --- ---- --- --- --- --- ---  //
        // --- --- --- --- REPRODUCCION --- --- --- ---  //
        // --- --- --- --- --- ---- --- --- --- --- ---  //
        std::poisson_distribution<int> poisson_males(   lambda_m ); // hijos machos
        std::poisson_distribution<int> poisson_females( lambda_f ); // hijas hembras

        for( int i = 0; i < cantidadParejas; i++ ){

            int padre_id = parejas[i].first;
            int madre_id = parejas[i].second;

            int cant_hijos_males = poisson_males(gen);
            int cant_hijos_females = poisson_females(gen);
            
            
            while( cant_hijos_males-- ){
                Pulpo hijo( next_id++, g + 1, MALE, padre_id, madre_id);
                bool autointerseccion = hijo.heredar( todosLosIndividuos[ padre_id ] , todosLosIndividuos[ madre_id ] );
                
                if( autointerseccion ){
                    insesto = true;
                    std::cout << "autointerseccion detectada en generacion " << g+1 << "\n";
                }
                todosLosIndividuos.push_back(hijo);
            }

            while( cant_hijos_females-- ){
                Pulpo hijo( next_id++, g + 1, FEMALE, padre_id, madre_id);
                bool autointerseccion = hijo.heredar( todosLosIndividuos[ padre_id ] , todosLosIndividuos[ madre_id ] );
                
                if( autointerseccion ){
                    insesto = true;
                    std::cout << "autointerseccion detectada en generacion " << g+1 << "\n";
                }
                todosLosIndividuos.push_back(hijo);
            }
                
  
            
        }

        generacionActual = generacionNext;
        generacionActualFin = todosLosIndividuos.size();

        
        g++;
    } 


    std::cout << "Simulacion terminada\nSe registraron " << todosLosIndividuos.size() << " individuos en " << g<< " generaciones\n"; 
    if( guardar ){
        guardarGraphviz(todosLosIndividuos, g);
    }
    
    return g;
}


// simulación  RÁPIDA usando la suma de Poisson
// Ideal para calcular probabilidades de extinción con miles de iteraciones
int DaltonWatsonSimpleFast(int size_inicial, double lambda, double probabilidadmale, 
                           int max_generations, std::vector<std::pair<ll, ll>>& historial) {
    
    // Limpiamos el historial para esta corrida
    historial.clear();
    historial.reserve(max_generations); // Reservamos memoria para evitar realojamientos

    double probabilidadfemale = 1.0 - probabilidadmale;
    std::random_device rd;
    std::mt19937 gen(rd());

    // --- Estado inicial ---
    ll current_males = size_inicial / 2;
    ll current_females = size_inicial - current_males;
    
    int g = 0;
    bool seExtinguio = false;

    // Guardamos el estado inicial (Generación 0)
    historial.push_back({current_males, current_females});

    while (g < max_generations && !seExtinguio) {
        
        // --- FORMAR PAREJAS ---
        // Usamos el modelo de apareamiento monogámico limitado por el sexo minoritario
        ll cantidadParejas = std::min(current_males, current_females);

        if (cantidadParejas <= 0) {
            seExtinguio = true;
            break;
        }

        // --- REPRODUCCIÓN (Suma de Poisson) ---
        // Basado en la propiedad: Sum(Pois(l1) + Pois(l2) ... Pois(ln)) = Pois(n * l)
        double lambda_total = cantidadParejas * lambda;
        
        std::poisson_distribution<ll> dist_males(lambda_total * probabilidadmale);
        std::poisson_distribution<ll> dist_females(lambda_total * probabilidadfemale);

        // Generar conteos de la siguiente generación de golpe
        current_males = dist_males(gen);
        current_females = dist_females(gen);

        // Registrar datos
        historial.push_back({current_males, current_females});
        
        if (current_males == 0 && current_females == 0) {
            seExtinguio = true;
        }

        g++;
    }

    return g;
}