#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

#include "SimulacionGaltonWatson.hpp"
typedef int64_t ll;

struct poblacion {
    long long males;
    long long females;
};



/// hacer una prueba de tiempo de muerte promedio
int main() {
    // --- configuracion de la simulación ---
    int size_inicial = 20;
    double lambda = 2.2; // promedio de hijos por pareja (Poisson)


    double probabilidadmale = 0.5; // probabilidad de que un hijo sea macho
    int max_generations = 100;

    std::vector<std::pair<ll, ll>> miHistorial;
    for(int i = 0; i < 20; i++) {
        DaltonWatsonSimpleFast(size_inicial, lambda, probabilidadmale, max_generations, miHistorial);
        if( true ){
            std::cout << "simlacion: " << i << " con " << miHistorial.size()  << "generaciones\n";
            std::cout << "gen \t Males \t Females\n";
            for( int j = 0; j< miHistorial.size(); j++ ){
                auto gen = miHistorial[j];
                std::cout << i << "\t" <<   gen.first <<  "\t" <<  gen.second <<'\n';
            }
            std::cout << "\n";
        }
    }
    return 0;
}