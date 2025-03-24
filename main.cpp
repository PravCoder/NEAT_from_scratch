#include <iostream>
using namespace std;
#include "Population.h" 

int main() {
    int population_size = 5;
    int network_inputs = 2;
    int network_outputs = 2;
    double crossover_rate = 0.3;

    Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "rand_connected"); // rand_connnected, fully_connected
    cout << "Initial Population:" << endl;
    p1.show_pop();

    cout << endl <<"---Mutating Genome - Add Connection:---" << endl;
    int genome_indx = 4;
    cout << "Before:" << endl;
    p1.genomes[genome_indx].show();
    cout << "After:" << endl;
    p1.mutation_add_connection(p1.genomes[genome_indx]);
    //p1.mutation_add_connection(p1.genomes[genome_indx]);
    //p1.mutation_add_connection(p1.genomes[genome_indx]);
    p1.genomes[genome_indx].show();


    cout << endl <<"\n---Mutating Genome - Add Node:---" << endl;
    genome_indx = 0;
    cout << "Before:" << endl;
    p1.genomes[genome_indx].show();
    p1.mutation_add_node(p1.genomes[genome_indx], true);
    cout << "After:" << endl;
    p1.genomes[genome_indx].show();

}

/*
g++ -o main main.cpp
./main
g++ -o main main.cpp && ./main
*/

/*
TODO:
- mutations, when to do?
- forward pass, fitness func
- create pop-func called evolve which is the evolutionary loop

BUG:
- link-to-split print is 0->0, cycle to itself input node when printing link-to-split in add node mutation func test
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