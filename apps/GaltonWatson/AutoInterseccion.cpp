#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <fstream>
#include <string>

#include "SimulacionGaltonWatson.hpp"

// int main( ){
//     /// --- parametros --- 
//     int size_inicial = 20; 
//     double lambda = 2.1;
//     double probabilidadmale = 0.5f;
//     int max_generations = 20; // <------------- Cuidado: un numero alto  desborda
//     std::cout << ejecutarSimulacion(size_inicial, lambda, probabilidadmale, max_generations, true) << '\n';
//     return 0;
// }

int main() {
    // Simulaciones por  experimento
    int iteraciones_por_punto = 50;
    int max_gen_limite = 50;
    
    // rango de variables para el plano X-Y
    std::vector<int> poblaciones_iniciales = {10, 20, 30, 40, 50, 60};
    std::vector<double> lambdas = {1.5, 2.0, 2.5, 3.0, 3.5,};

    std::ofstream data_file("apps/GaltonWatson/datos/experimento_incesto2.txt");
    data_file << "Poblacion_Inicial Lambda Gen_Media_Incesto\n";

    for(int n0 : poblaciones_iniciales) {
        for(double l : lambdas) {
            double suma_generaciones = 0;
            
            for(int i = 0; i < iteraciones_por_punto; i++) {
                suma_generaciones += ejecutarSimulacion(n0, l, 0.5, max_gen_limite);
            }
            
            double promedio = suma_generaciones / iteraciones_por_punto;
            data_file << n0 << " " << l << " " << promedio << "\n";
            
            std::cout << "Terminado: N=" << n0 << " Lambda=" << l << " -> Promedio: " << promedio << "\n";
        }
    }
    data_file.close();
    return 0;
}

