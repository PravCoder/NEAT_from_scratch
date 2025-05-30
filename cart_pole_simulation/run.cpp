#include <iostream>
using namespace std;
#include "../Population.h"   
#include "CartPoleEnvironment.h" 
#include <sstream>

int main() {
    srand(time(NULL)); 

    int population_size = 150; // make sure even
    int network_inputs = 4;
    int network_outputs = 1;  // make sure this is set for one-hot-encoding/single-output xor
    double crossover_rate = 0.75;
    int num_generations = 100;
    int tournament_size = 3;


    // runs the cart-pole neat evolution
    Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "fully_connected", num_generations, tournament_size); // rand_connnected, fully_connected
    p1.create_population("fully_connected");
    p1.evolutionary_loop_cartpole();

    
}


/*
RUN: 
g++ -std=c++11 -o run run.cpp && ./run
*/