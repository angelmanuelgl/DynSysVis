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
    /// --- parametros --- 
    int max_generations = 7; // Cuidado: un numero alto genera archivos gigantes
    double lambda = 2;
    int next_id = 0;
    double probabilidadmale = 0.5f;

    std::vector<Pulpo> todosLosIndividuos;
    std::random_device rd;
    std::mt19937 gen(rd());

    //  --- Poblacion Inicial --- 
    int init_size = 6; // 2 machos, 2 hembras
    for( int i = 0; i < init_size; i++){
        Sex s = (i % 2 == 0) ? MALE : FEMALE;
        // emplace_back enlugar de pus_back
        todosLosIndividuos.emplace_back(next_id++, 0, s, -1, -1);
    }

    // --- simulacion ---
    int cnt_males, cnt_females;
    int generacionActual = 0;
    int generacionActualFin = todosLosIndividuos.size();
    int g = 0;
    
    
    for( ; g < max_generations; g++ ){
        std::vector<int> males, females;
        
        //  --- separar machos y hembras--- 
        // la generacion actual esta justo en los indices [generacionActual, size ]
        for( int i = generacionActual; i < generacionActualFin; i++ ){
            if( todosLosIndividuos[i].sex == MALE) males.push_back(todosLosIndividuos[i].id);
            else females.push_back(todosLosIndividuos[i].id);
        }

        //TODO: min, max, f, m, otros ejemplos
        int num_couples = std::min(males.size(), females.size());
        if( num_couples == 0) break;

        int generacionNext = todosLosIndividuos.size();

        // --- reproduccion ---
        // nota: usar poisson y luego elegir con bernolli === usar dos poisson
        // pero no en general con otras variables aleatorias
        std::poisson_distribution<int> poisson(lambda); // hijos
        std::bernoulli_distribution bernoulli( probabilidadmale ); // macho o empra
        

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

        
    }


    // exportar a formato Graphviz (.dot)
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