#include <iostream>
using namespace std;
#include "../Population.h"   
#include "CartPoleEnvironment.h" 
#include <sstream>

int main() {
    int population_size = 150; // make sure even
    int network_inputs = 2;
    int network_outputs = 1;  // make sure this is set for one-hot-encoding/single-output xor
    double crossover_rate = 0.75;
    int num_generations = 100;
    int tournament_size = 3;


    CartPoleEnvironment env;
    env.run_cartpole_neat_evolution(population_size, network_inputs, network_outputs, crossover_rate, num_generations, tournament_size);
}


/*
RUN: 
g++ -std=c++11 -o run run.cpp && ./run
*/