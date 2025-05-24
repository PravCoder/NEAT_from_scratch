#include <iostream>
using namespace std;
#include "Population.h" 
#include <sstream>


void print_vector(const vector<double>& vec) {
    cout << "[ ";
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";  // avoid trailing comma
    }
    cout << " ]" << endl;
}

string vector_to_string(const std::vector<double>& vec) {
    std::ostringstream oss;
    oss << "[ ";
    for (size_t i = 0; i < vec.size(); i++) {
        oss << vec[i];
        if (i < vec.size() - 1) oss << ", ";
    }
    oss << " ]";
    return oss.str();
}

// Testing Hyperparameters
// int population_size = 6; // make sure even
// int network_inputs = 2;
// int network_outputs = 2;
// double crossover_rate = 0.3;
// int num_generations = 10;
// int tournament_size = 2;

// XOR Hyperparameters
int population_size = 150; // make sure even
int network_inputs = 2;
int network_outputs = 1;  // make sure this is set for one-hot-encoding/single-output xor
double crossover_rate = 0.75;
int num_generations = 200;
int tournament_size = 3;

Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "fully_connected", num_generations, tournament_size); // rand_connnected, fully_connected
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

// Single-output XOR
vector<vector<double>> X1 = {{0,0}, {0,1}, {1,0}, {1,1}};
vector<vector<double>> Y1 = {{0}, {1}, {1}, {0}};

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

void forward_prop_single_example_xor_test_one_hot() { 
    // input values & labels
    vector<double> x_1 = {0, 0};
    vector<double> x_2 = {0, 1};

    vector<double> y_1 = {0, 1};
    vector<double> y_2 = {1, 0};



    cout << "\n---Forward Prop Single Example xor Test One Hot Encoding---" << endl;
    int genome_indx = 2;
    Genome& genome = p1.get_best_network();
    genome.show();
    // p1.mutation_add_node(genome, true);
    genome.show_weights();                                                // output may be [0.5, 0.5] if no input-node is connected to anything like a hidden or output node for sigmoid
    vector<double> y_hat = genome.forward_propagate_single_example(x_2); // something may be wrong with forward prop where the activations are always 0.5 because some input isnt connected to a output, or output doesnt have any connections to it. Outputs activations array maybe out of order 
    
    cout << "input: ";
    print_vector(x_2);
    cout << "predicted output (d,s): ";
    print_vector(y_hat);  // can check calculation by hand using network topology, weights, activation funcs, and input example.
    cout << "actual output:";
    print_vector(y_2);
}

void forward_prop_single_example_xor_single_output() { 
    // input values & labels
    vector<double> x_1 = {0, 0};
    vector<double> y_1 = {0};

    vector<double> x_2 = {0, 1};
    vector<double> y_2 = {1};

    vector<double> x_3 = {1, 0};
    vector<double> y_3 = {1};

    vector<double> x_4 = {1, 1};
    vector<double> y_4 = {0};



    cout << "\n---Forward Prop Single Example xor Test Single Output---" << endl;
    int genome_indx = 2;
    Genome& genome = p1.get_best_network();
    genome.show();
    // p1.mutation_add_node(genome, true);
    genome.show_weights();                                                // output may be [0.5, 0.5] if no input-node is connected to anything like a hidden or output node for sigmoid
    vector<double> y_hat1 = genome.forward_propagate_single_example(x_1); 
    vector<double> y_hat2 = genome.forward_propagate_single_example(x_2); 
    vector<double> y_hat3 = genome.forward_propagate_single_example(x_3); 
    vector<double> y_hat4 = genome.forward_propagate_single_example(x_4); 
    
    cout << "input: " << vector_to_string(x_1) << " actual: " << vector_to_string(y_1) << " pred: " << vector_to_string(y_hat1) << endl;
    cout << "input: " << vector_to_string(x_2) << " actual: " << vector_to_string(y_2) << " pred: " << vector_to_string(y_hat2) << endl;

    cout << "input: " << vector_to_string(x_3) << " actual: " << vector_to_string(y_3) << " pred: " << vector_to_string(y_hat3) << endl;
    cout << "input: " << vector_to_string(x_4) << " actual: " << vector_to_string(y_4) << " pred: " << vector_to_string(y_hat4) << endl;
    
}

void fitness_func_xor_test() {
    int genome_indx = 2;
    Genome& genome = p1.genomes[genome_indx];
    double fitness = p1.compute_fitness_xor(X, Y, genome);
    cout << "\n---Fitness Function XOR Test---" << endl;
    genome.show();
    cout << "fitness: " << genome.fitness  << endl;
}


/*
This test is to manually create a netowrk that is the solution of XOR to test if everything else besides the evolution process is working
*/
void test_manual_xor() {
    cout << "\n---Testing Manual XOR Network (Simplified)---" << endl;
    
    Genome manual(2, 1);  // 2 inputs, 1 output
    
    // Create nodes
    manual.nodes.push_back(NodeGene(0, "input"));   // Input 1
    manual.nodes.push_back(NodeGene(1, "input"));   // Input 2
    manual.nodes.push_back(NodeGene(2, "hidden"));  // Hidden 1
    manual.nodes.push_back(NodeGene(3, "hidden"));  // Hidden 2  
    manual.nodes.push_back(NodeGene(4, "output"));  // Output
    
    // Classic XOR solution with well-tested weights
    // This creates: Hidden1 = input1 OR input2, Hidden2 = input1 AND input2
    // Output = Hidden1 AND NOT Hidden2 = XOR
    
    manual.links.push_back(LinkGene(0, 2, 6.0, true, 0));    // Input1 -> Hidden1 (OR gate part 1)
    manual.links.push_back(LinkGene(1, 2, 6.0, true, 1));    // Input2 -> Hidden1 (OR gate part 2)
    manual.links.push_back(LinkGene(0, 3, 6.0, true, 2));    // Input1 -> Hidden2 (AND gate part 1)
    manual.links.push_back(LinkGene(1, 3, 6.0, true, 3));    // Input2 -> Hidden2 (AND gate part 2)
    manual.links.push_back(LinkGene(2, 4, 6.0, true, 4));    // Hidden1 -> Output (positive)
    manual.links.push_back(LinkGene(3, 4, -12.0, true, 5));  // Hidden2 -> Output (strong negative)
    
    // Set biases
    manual.nodes[2].bias = -3.0;   // OR gate: activate when at least one input is high
    manual.nodes[3].bias = -9.0;   // AND gate: activate only when both inputs are high
    manual.nodes[4].bias = -3.0;   // Output bias
    
    // Set input/output node IDs
    manual.set_input_output_node_ids();


    
    // Debug: Show intermediate activations
    cout << "Manual network structure:" << endl;
    manual.show();
    manual.show_weights();
    cout << "fitness: " << p1.compute_fitness_xor(X1, Y1, manual) << endl;
    
    // Test with detailed breakdown
    vector<vector<double>> inputs = {{0,0}, {0,1}, {1,0}, {1,1}};
    vector<int> expected = {0, 1, 1, 0};
    
    cout << "\nDetailed testing:" << endl;
    for (int i = 0; i < inputs.size(); i++) {
        vector<double> output = manual.forward_propagate_single_example(inputs[i]);
        
        // Show intermediate node activations for debugging
        cout << "Input: [" << inputs[i][0] << "," << inputs[i][1] << "] ";
        cout << "Hidden1: " << manual.nodes[2].activation << " ";
        cout << "Hidden2: " << manual.nodes[3].activation << " ";
        cout << "Output: " << output[0] << " ";
        
        int predicted = (output[0] >= 0.5) ? 1 : 0;
        cout << "Predicted: " << predicted << " ";
        cout << "Expected: " << expected[i] << " ";
        cout << ((predicted == expected[i]) ? "✓" : "✗") << endl;
    }
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
    // p1.genomes[0].show_weights();
    p1.evolutionary_loop(X1, Y1);  // make sure this is the correct dataset either one-hot-encoding/single-output
    forward_prop_single_example_xor_single_output();
    // p1.show_pop();
    // crossover_test();


    // test_manual_xor();


    return 0;
}

/*

---Forward Prop Single Example xor Test---
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out, 4
   links: 
   0->2[IN=0]d
   0->3[IN=3]d
   0->4[IN=6]e
   4->2[IN=5]e
   weights:
   [IN=6]-2
   [IN=3]-0.274386
   [IN=5]-0.451648
   [IN=0]--0.211217
input: [ 0, 1 ]
predicted output: [ 0.5, 0.5 ]
actual output:[ 1, 0 ]
*/



/*
RUN: 
g++ -std=c++11 -o tests tests.cpp && ./tests

g++ -o tests tests.cpp
./tests
g++ -o tests tests.cpp && ./tests
g++ -std=c++11 -o tests tests.cpp && ./tests
g++ -std=c++11 -Wall -o tests tests.cpp && ./tests

*/