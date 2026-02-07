#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

struct poblacion {
    long long males;
    long long females;
};

/// BGWP

/// hacer una prueba de tiempo de muerte promedio
int main() {
    // --- configuracion de la simulación ---
    int generations = 80;
    long long ini_males = 10;
    long long ini_females = 10;
    double lambda = 2.0; // promedio de hijos por pareja (Distribución Poisson)
    double male_prob = 0.5; // probabilidad de que un hijo sea macho

    // numeros aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    

    // poblacion actual
    

    // std::cout << "Gen\tMachos\tHembras\tParejas\n";
    // std::cout << "---------------------------------\n";
    long double media = 0;
    int iter = 50;
    int xd = iter;
    while( xd-- ){ 
    poblacion current = {ini_males, ini_females};        
    int g = 0;
    for( ; g <= generations; g++ ){
        //  --- Determinar parejas --- 
        long long couples = std::min(current.males, current.females);


        // usar https://edotor.net/ para visualizar
        std::cout << g << "\t" << current.males << "\t" << current.females << "\t" << couples << "\n";
        if( couples == 0 ){
            // std::cout << "--- EXTINCIÓN ---\n";
            break;
        }

        // ---- Reproduccion ---
        // cuantos hijos en total tienen todas las parejas?
        // La suma de N variables Poisson(lambda) es una Poisson(N * lambda)
        std::poisson_distribution<long long> offspring_dist(couples * lambda);
        long long total_offspring = offspring_dist(gen);

        // diferenciacion de sexo: ¿Cuántos son machos?
        // Usamos una distribución Binomial para repartir el total de hijos
        std::binomial_distribution<long long> gender_dist(total_offspring, male_prob);
        
        current.males = gender_dist(gen);
        current.females = total_offspring - current.males;
    }
    
    

    // solo imrpimir los primeros 1000 datos
    if( xd <  1000 ){
        if( xd%35 == 0 ) std::cout << '\n';
        if( g < generations ) std::cout << g << ' ';
    }
    // contar cuantos van
    if( g >= generations){
        std:: cout << " ++  \n\n";
    }  
    // calcular media
    media += g;

    }   

    std::cout << '\n' << media/(iter) << '\n';
    return 0;
}