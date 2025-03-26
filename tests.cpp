#include <iostream>
using namespace std;
#include "Population.h" 


void print_vector(const vector<double>& vec) {
    cout << "[ ";
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";  // Avoid trailing comma
    }
    cout << " ]" << endl;
}


int population_size = 5;
int network_inputs = 2;
int network_outputs = 2;
double crossover_rate = 0.3;
int num_generations = 5;
Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "rand_connected", num_generations); // rand_connnected, fully_connected
vector<vector<double>> X = {
    {0, 0},  
    {0, 1},  
    {1, 0},  
    {1, 1}  
};
vector<vector<double>> Y = {
    {0},     
    {1},     
    {1},    
    {0}     
};


void add_connection_test(){
    cout << endl <<"---Mutating Genome - Add Connection:---" << endl;
    int genome_indx = 4;
    cout << "Before:" << endl;
    p1.genomes[genome_indx].show();
    cout << "After:" << endl;
    p1.mutation_add_connection(p1.genomes[genome_indx]);
    //p1.mutation_add_connection(p1.genomes[genome_indx]);
    //p1.mutation_add_connection(p1.genomes[genome_indx]);
    p1.genomes[genome_indx].show();
}

void add_node_test() {
    cout << endl <<"\n---Mutating Genome - Add Node:---" << endl;
    int genome_indx = 4;
    genome_indx = 0;
    cout << "Before:" << endl;
    p1.genomes[genome_indx].show();
    p1.mutation_add_node(p1.genomes[genome_indx], true);
    p1.mutation_add_node(p1.genomes[genome_indx], true);
    cout << "After:" << endl;
    p1.genomes[genome_indx].show();
}

void weight_mutation_test() {
    cout << endl <<"\n---Mutating Genome - Weight Modification:---" << endl;
    int genome_indx = 4;
    genome_indx = 2;
    cout << "Before:" << endl;
    p1.genomes[genome_indx].show();
    p1.mutation_modify_weights(p1.genomes[genome_indx], true);
    cout << "After:" << endl;
    p1.genomes[genome_indx].show();
}

void forward_prop_single_example_xor_test() { 
    // input values & labels
    vector<double> x_1 = {0, 0};
    vector<double> x_2 = {0, 1};

    vector<double> y_1 = {0};
    vector<double> y_2 = {1};

    cout << "\n---Forward Prop Single Example xor Test---" << endl;
    int genome_indx = 2;
    Genome& genome = p1.genomes[genome_indx];
    genome.show();
    // p1.mutation_add_node(genome, true);
    genome.show_weights();                                                // output may be [0.5, 0.5] if no input-node is connected to anything like a hidden or output node for sigmoid
    vector<double> y_hat = genome.forward_propagate_single_example(x_2); // something may be wrong with forward prop where the activations are always 0.5 because some input isnt connected to a output, or output doesnt have any connections to it. Outputs activations array maybe out of order 
    print_vector(y_hat);  // can check calculation by hand using network topology, weights, activation funcs, and input example.
    
}

void fitness_func_xor_test() {
    int genome_indx = 2;
    Genome& genome = p1.genomes[genome_indx];
    double fitness = p1.compute_fitness_xor(X, Y, genome);
    cout << "\n---Fitness Function XOR Test---" << endl;
    genome.show();
    cout << "fitness: " << genome.fitness  << endl;
}

int main() {
    // create population initial population firstfirst
    // p1.create_population("rand_connected");   // when testing just manually create initial pop
    // p1.show_pop();
    
    // tests - pre evolution first gen so fitness of a genome with less links might be higher than a genome with more weights
    // add_connection_test();
    // add_node_test();
    // weight_mutation_test();
    // forward_prop_single_example_xor_test();
    // fitness_func_xor_test();
    
    p1.evolutionary_loop(X, Y);


    return 0;
}



/*
g++ -o tests tests.cpp
./tests
g++ -o tests tests.cpp && ./tests
g++ -std=c++11 -o tests tests.cpp && ./tests
*/