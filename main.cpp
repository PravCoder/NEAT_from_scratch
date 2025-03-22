#include <iostream>
using namespace std;
#include "Population.h" 

int main() {
    int population_size = 5;
    int network_inputs = 2;
    int network_outputs = 2;
    double crossover_rate = 0.3;

    Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "rand_connected"); // rand_connnected, fully_connected
    p1.show_pop();

    cout << endl <<"Mutated Genome - Add Connection:" << endl;
    cout << "Before:" << endl;
    p1.genomes[3].show();
    cout << "After:" << endl;
    p1.mutation_add_connection(p1.genomes[4]);
    p1.genomes[3].show();
}

/*
g++ -o main main.cpp
./main
*/

/*
TODO:
- mutations, when to do?
- forward pass, fitness func
- create pop-func called evolve which is the evolutionary loop
*/

/*
*****NOTES****

Even when randomly-connected for intailizing first generation of genomes, same links should have innovation numbers across genomes:
Population size: 5
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->2[IN=0]
   1->2[IN=1]
   1->3[IN=2]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->3[IN=2]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->3[IN=3]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->2[IN=0]
   0->3[IN=3]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->3[IN=2]

*/