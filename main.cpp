#include <iostream>
using namespace std;
#include "Population.h" 

int main() {
    int population_size = 5;
    int network_inputs = 2;
    int network_outputs = 2;
    double crossover_rate = 0.3;

    Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "fully_connected"); // rand_connnected, fully_connected
    p1.show_pop();
    // in initial population innovation numbers are just increment when they all should be the same. 
}

/*
g++ -o main main.cpp
./main
*/

/*
TODO:
- crossover func
*/