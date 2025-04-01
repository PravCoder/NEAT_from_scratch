#include <iostream>
using namespace std;
#include "Population.h" 


void print_vector(const vector<double>& vec) {
    cout << "[ ";
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";  // avoid trailing comma
    }
    cout << " ]" << endl;
}

// Testing Hyperparameters
// int population_size = 6; // make sure even
// int network_inputs = 2;
// int network_outputs = 2;
// double crossover_rate = 0.3;
// int num_generations = 10;
// int tournament_size = 2;

// XOR Hyperparameters
int population_size = 200; // make sure even
int network_inputs = 2;
int network_outputs = 2;
double crossover_rate = 0.7;
int num_generations = 200;
int tournament_size = 3;

Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "rand_connected", num_generations, tournament_size); // rand_connnected, fully_connected
vector<vector<double>> X = {
    {0, 0},  
    {0, 1},  
    {1, 0},  
    {1, 1}  
};
vector<vector<double>> Y = {  
    {0, 1},  
    {1, 0},    // first node is different, second node is same, also for predictions
    {1, 0},  
    {0, 1}    
};

void crossover_test() {
    int i = 0;
    int j = 2;
    cout << endl <<"---Crossover Two Parent Genomes Test:---" << endl;
    Genome offspring = p1.crossover_genomes(p1.genomes[i], p1.genomes[j]);
    cout << "Parent 1: " << p1.genomes[i].fitness << endl;
    p1.genomes[i].show();
    cout << "Parent 2: " << p1.genomes[j].fitness  << endl;
    p1.genomes[j].show();
    cout << "Offspring: " << endl;
    offspring.show(); 
    
}

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

    vector<double> y_1 = {0, 1};
    vector<double> y_2 = {1, 0};

    cout << "\n---Forward Prop Single Example xor Test---" << endl;
    int genome_indx = 2;
    Genome& genome = p1.genomes[genome_indx];
    genome.show();
    // p1.mutation_add_node(genome, true);
    genome.show_weights();                                                // output may be [0.5, 0.5] if no input-node is connected to anything like a hidden or output node for sigmoid
    vector<double> y_hat = genome.forward_propagate_single_example(x_2); // something may be wrong with forward prop where the activations are always 0.5 because some input isnt connected to a output, or output doesnt have any connections to it. Outputs activations array maybe out of order 
    
    cout << "input: ";
    print_vector(x_2);
    cout << "predicted output: ";
    print_vector(y_hat);  // can check calculation by hand using network topology, weights, activation funcs, and input example.
    cout << "actual output:";
    print_vector(y_2);
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
    srand(time(NULL));

    // Pre-Evolution Tests: first gen so fitness of a genome with less links might be higher than a genome with more weights
    // create population initial population first
    // p1.create_population("rand_connected");   // when testing just manually create initial pop
    // p1.show_pop();
    // // crossover_test();
    
    // add_connection_test();
    // add_node_test();
    // weight_mutation_test();
    // forward_prop_single_example_xor_test();
    // fitness_func_xor_test();
    
    // Evolutionary Tests - post evolution tests
    p1.create_population("rand_connected"); 
    p1.show_pop();
    p1.evolutionary_loop(X, Y);
    
    // forward_prop_single_example_xor_test();
    // p1.show_pop();
    // crossover_test();


    return 0;
}



/*
g++ -o tests tests.cpp
./tests
g++ -o tests tests.cpp && ./tests
g++ -std=c++11 -o tests tests.cpp && ./tests
*/