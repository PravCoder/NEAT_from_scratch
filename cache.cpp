#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <random>
#include <ctime>
#include <utility>
#include <iostream>
#include "Genome.h"

using namespace std;

/*
Represents a collection of genomes/individuals. 1 Population object is created in main and is used, cleaned, created for all generations.
*/
class Population {
public:
    static int next_innovation_num; // the current latest innovation number, increment to get next IN
    vector<Genome> genomes;  // arr of genome-objs for cur-population
    int population_size;
    double crossover_rate; // probability value between 0-1
    int num_inputs;
    int num_outputs;
    string initializer;

    // hashmap where each key is <source-node, target-node> connection value is its innovation-number
    map<pair<int, int>, int> innovation_nums_map;   
    
    int num_generations;
    double best_fitness;
    double avr_fitness;
    int tournament_size;

    int num_weight_mutations = 0;
    int num_add_node_mutations = 0;
    int num_add_connection_mutations = 0;  // more gen stats TBD

    Population(int size, int numInputs, int numOutputs, double cr, string initializer, int num_generations, int tournament_size) : 
        num_inputs(numInputs), 
        num_outputs(numOutputs), 
        population_size(size), 
        crossover_rate(cr), 
        initializer(initializer), 
        num_generations(num_generations), 
        tournament_size(tournament_size),
        best_fitness(0.0),
        avr_fitness(0.0) {}
    
    // creates genome-objs for initial-first-generation
    void create_population(string initializer) {
        genomes.clear(); // Clear any existing genomes first
        // iterate the population-size
        for (int i=0; i<population_size; i++) {
            Genome cur_genome = Genome(num_inputs, num_outputs);  // create genome-obj with num-inputs-outputs
            genomes.push_back(cur_genome);                        // add genome-obj to genomes-arr
            if (initializer == "fully_connected") {
                initialize_first_gen_genome_fully_connected(genomes.back()); // get the genome that we pushed to vector not the local one
            }
            else if (initializer == "rand_connected") {
                initialize_first_gen_genome_randomly_connected(genomes.back());
            }
            else {
                cerr << "Unknown initializer: " << initializer << endl;
                // Default to fully connected
                initialize_first_gen_genome_fully_connected(genomes.back());
            }
        }
    }

    // either gets existing or creates a new innovation-number based on given connection
    int get_innovation_number(int source_id, int target_id) {
        pair<int, int> connection = make_pair(source_id, target_id);
        
        // if given connection exists in our map, return that innovation number value
        if (innovation_nums_map.find(connection) != innovation_nums_map.end()) { 
            return innovation_nums_map[connection];
        } else {  // if given connection doesnt exist in our map, create a new innovation number and add it to our map. 
            int new_innov_num = next_innovation_num++;
            innovation_nums_map[connection] = new_innov_num; // add (connection, innov-num) pair to map
            return innovation_nums_map[connection];
        }
    }

    // given empty-genome-obj initializes it by creating its nodes and fully connecting them
    void initialize_first_gen_genome_fully_connected(Genome& genome) {
        // Create input nodes
        for (int i=0; i<num_inputs; i++) {
            genome.nodes.push_back(NodeGene(i, "input"));
        }
        // Create output nodes
        for (int i=0; i<num_outputs; i++) {
            genome.nodes.push_back(NodeGene(num_inputs+i, "output"));
        }
        genome.set_input_output_node_ids();

        // Connect every input to every output
        for (int i=0; i<num_inputs; i++) {
            for (int j=0; j<num_outputs; j++) {
                int input_node_id = i; 
                int output_node_id = num_inputs + j;
                double weight = (rand() % 200 - 100) / 100.0;
                int innov_num = get_innovation_number(input_node_id, output_node_id);
                genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
            }
        }
    }

    // given empty-genome-obj initializes it with random connections
    void initialize_first_gen_genome_randomly_connected(Genome& genome, double connection_prob = 0.7) {
        // Create input and output nodes
        for (int i=0; i<num_inputs; i++) {
            genome.nodes.push_back(NodeGene(i, "input"));
        }
        for (int i=0; i<num_outputs; i++) {
            genome.nodes.push_back(NodeGene(num_inputs+i, "output"));
        }
        
        genome.set_input_output_node_ids();

        // Connect inputs to outputs with probability
        for (int i=0; i<num_inputs; i++) {
            for (int j=0; j<num_outputs; j++) {
                if ((double)rand() / RAND_MAX < connection_prob) {
                    int input_node_id = i;
                    int output_node_id = num_inputs+j;
                    double weight = (rand() % 200 - 100) / 100.0;
                    int innov_num = get_innovation_number(input_node_id, output_node_id);
                    genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
                }
            }
        }

        // Make sure each output has at least one connection to prevent dead outputs
        for (int j=0; j<num_outputs; j++) {
            int output_node_id = num_inputs+j;
            bool has_connection = false;
            
            // Check if this output already has a connection
            for (auto& link : genome.links) {
                if (link.output_node == output_node_id) {
                    has_connection = true;
                    break;
                }
            }
            
            // if cur-output-node doesnt have a connection to it, create a random connection to cur-output-node
            if (has_connection == false) {
                int input_node_id = genome.input_node_ids[rand() % input_node_ids.size()];  // get a randomt input-node-id
                double weight = get_random_gaussian_weight();  // get random weight
                int innov_num = get_innovation_number(input_node_id, output_node_id); // get next innovation number for this new connection
                // add this new link with  random-input-node-id and cur-output-node
                genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
            }
        }
    }

    void show_pop() {
        cout << "Population size: " << population_size << endl;
        for (int i=0; i<genomes.size(); i++) {
            cout << "Genome " << i << " (fitness: " << genomes[i].fitness << ")" << endl;
            genomes[i].show();
            cout << endl;
        }
    }

    Genome crossover_genomes(Genome& parent1, Genome& parent2) {
        // Choose the better parent
        Genome* better_parent_ptr;
        Genome* lesser_parent_ptr;
        
        if (parent1.fitness > parent2.fitness) {
            better_parent_ptr = &parent1;
            lesser_parent_ptr = &parent2;
        } else if (parent2.fitness > parent1.fitness) {
            better_parent_ptr = &parent2;
            lesser_parent_ptr = &parent1;
        } else {
            // Equal fitness, choose randomly
            if (rand() % 2 == 0) {
                better_parent_ptr = &parent1;
                lesser_parent_ptr = &parent2;
            } else {
                better_parent_ptr = &parent2;
                lesser_parent_ptr = &parent1;
            }
        }
        
        Genome& better_parent = *better_parent_ptr;
        Genome& lesser_parent = *lesser_parent_ptr;

        // Initialize offspring genome
        Genome offspring = Genome(better_parent.num_inputs, better_parent.num_outputs);
        
        // Inherit all nodes from better parent
        for (int i=0; i<better_parent.nodes.size(); i++) {
            offspring.nodes.push_back(better_parent.nodes[i]);
        }
        
        // Create innovation map for lesser parent
        lesser_parent.create_innovation_num_to_link_gene_map();
        
        // Process links
        for (int i=0; i<better_parent.links.size(); i++) {
            LinkGene& better_link = better_parent.links[i];

            // Check if this link exists in lesser parent (matching gene)
            if (lesser_parent.genes_map.find(better_link.innovation_num) != lesser_parent.genes_map.end()) {
                // Matching gene: randomly inherit from either parent
                if (rand() % 2 == 0) {
                    offspring.links.push_back(better_link);
                } else {
                    offspring.links.push_back(lesser_parent.genes_map[better_link.innovation_num]);
                }
            } else {
                // Disjoint/excess gene: inherit from better parent
                offspring.links.push_back(better_link);
            }
        }

        // Handle edge case of empty links
        if (offspring.links.empty()) {
            offspring.links = better_parent.links;
        }
        
        offspring.set_input_output_node_ids();
        return offspring;
    }

    void mutation_add_connection(Genome& offspring) {
        if (offspring.nodes.empty()) {
            cerr << "Cannot add connection to empty network" << endl;
            return;
        }
        
        // Number of possible connections
        int num_hidden = offspring.nodes.size() - (num_inputs + num_outputs);
        
        // Get valid nodes for source and target
        vector<int> input_hidden_node_indices = offspring.get_input_and_hidden_nodes_indicies();
        vector<int> hidden_output_node_indices = offspring.get_hidden_and_output_nodes_indicies();
        
        if (input_hidden_node_indices.empty() || hidden_output_node_indices.empty()) {
            cerr << "No valid source or target nodes for new connection" << endl;
            return;
        }

        const int MAX_ATTEMPTS = 20;
        
        for (int attempt=0; attempt<MAX_ATTEMPTS; attempt++) {
            // Randomly select source and target nodes
            int source_idx = input_hidden_node_indices[rand() % input_hidden_node_indices.size()];
            int target_idx = hidden_output_node_indices[rand() % hidden_output_node_indices.size()];

            NodeGene& source_node = offspring.nodes[source_idx];
            NodeGene& target_node = offspring.nodes[target_idx];

            // Prevent cycles in hidden layer connections
            if (source_node.type == "hidden" && target_node.type == "hidden" && source_node.id >= target_node.id) {
                continue;
            }

            // Check if connection already exists
            if (offspring.does_connection_exist(source_node.id, target_node.id)) {
                continue;
            }

            // Create new connection
            int new_innovation_num = get_innovation_number(source_node.id, target_node.id);
            double new_weight = (rand() % 200 - 100) / 100.0;
            offspring.links.push_back(LinkGene(source_node.id, target_node.id, new_weight, true, new_innovation_num));
            num_add_connection_mutations++;
            return;
        }
    }

    void mutation_add_node(Genome& offspring, bool show_info = false) {
        if (show_info) {
            cout << "----Mutation Add Node----:" << endl;
            cout << "before mutation: " << endl;
            offspring.show();
        }

        // Handle completely empty networks by initializing them
        if (offspring.nodes.empty()) {
            cerr << "Network has no nodes. Initializing with basic structure." << endl;
            initialize_first_gen_genome_fully_connected(offspring);
            offspring.set_input_output_node_ids();
            return; // Skip mutation this time, just fix the network
        }
        
        // Handle networks with nodes but no links
        if (offspring.links.empty()) {
            cerr << "Network has no connections. Adding default connections." << endl;
            
            // Ensure input/output node IDs are set
            offspring.set_input_output_node_ids();
            
            // If we have input and output nodes, add at least one connection
            if (!offspring.input_node_ids.empty() && !offspring.output_node_ids.empty()) {
                int input_id = offspring.input_node_ids[0];
                int output_id = offspring.output_node_ids[0];
                double weight = (rand() % 200 - 100) / 100.0;
                int innov_num = get_innovation_number(input_id, output_id);
                offspring.links.push_back(LinkGene(input_id, output_id, weight, true, innov_num));
                return; // Skip node addition, just added a link
            } else {
                // Critical error - we have nodes but no inputs/outputs
                cerr << "Critical error: Network has nodes but input/output IDs are not set." << endl;
                initialize_first_gen_genome_fully_connected(offspring);
                offspring.set_input_output_node_ids();
                return;
            }
        }

        // Now we can safely proceed with adding a node
        int rand_link_index = rand() % offspring.links.size();
        LinkGene& link_to_split = offspring.links[rand_link_index];
        
        // Disable the chosen link
        link_to_split.enabled = false;
        
        // Create a new hidden node
        int new_node_id = offspring.get_next_node_id();
        NodeGene new_node = NodeGene(new_node_id, "hidden");

        // Create two new connections
        int innov_num1 = get_innovation_number(link_to_split.input_node, new_node_id);
        int innov_num2 = get_innovation_number(new_node_id, link_to_split.output_node);
        
        LinkGene link_1 = LinkGene(link_to_split.input_node, new_node_id, 1.0, true, innov_num1);
        LinkGene link_2 = LinkGene(new_node_id, link_to_split.output_node, link_to_split.weight, true, innov_num2);

        // Add new node and links
        offspring.nodes.push_back(new_node);
        offspring.links.push_back(link_1);
        offspring.links.push_back(link_2);
        
        num_add_node_mutations++;

        if (show_info) {
            cout << "link_to_split: ";
            link_to_split.show();
            cout << "new-node: " << new_node.id << endl;
            cout << "after mutation: " << endl;
            offspring.show();
        }
    }

    void mutation_modify_weights(Genome& offspring, bool show_info = false) {
        if (offspring.links.empty()) {
            return;
        }
        
        if (show_info) {
            cout << "---- Mutation Weights----:" << endl;
            cout << "before: " << endl;
            offspring.show();
        }
        
        for (int i=0; i<offspring.links.size(); i++) {
            double before_weight = offspring.links[i].weight;
            double rand_weight = get_random_gaussian_weight();
            offspring.links[i].weight += rand_weight;
            
            if (show_info) {
                cout << "link " << i << " before weight: " << before_weight 
                     << ", after weight: " << offspring.links[i].weight << endl;
            }
        }
        
        num_weight_mutations++;
        
        if (show_info) {
            cout << "after: " << endl;
            offspring.show();
        }
    }

    // XOR Fitness computation
    double compute_fitness_xor(vector<vector<double>>& X, vector<vector<double>>& Y, Genome& genome) {
        // Check for empty networks
        if (genome.links.empty() || genome.nodes.empty()) {
            return 0.0001;  // Penalize empty networks
        }

        double total_error = 0.0;
        
        // Process each example
        for (int i=0; i<X.size(); i++) {
            vector<double>& cur_example = X[i];
            vector<double> prediction = genome.forward_propagate_single_example(cur_example);
            
            // Handle error if prediction is empty
            if (prediction.empty()) {
                return 0.0001;
            }
            
            double example_error = 0.0;
            for (int j=0; j<prediction.size(); j++) {
                double error = prediction[j] - Y[i][j];
                example_error += error * error; // Squared error
            }
            
            total_error += example_error;
        }

        double fitness = 1.0 / (1.0 + total_error); // Convert error to fitness (higher is better)
        genome.fitness = fitness;
        return fitness;
    }
    
    /*
    Loop to compute fitness and create next generation of genomes based on best performing genomes
    of previous population.
    */
    void evolutionary_loop(vector<vector<double>>& X, vector<vector<double>>& Y) {
        create_population(initializer);  // Create initial population
        
        for (int gen=0; gen<num_generations; gen++) {
            cout << "-----Generation #" << gen << "-----" << endl;
            check_empty_networks();

            // Compute fitness for each genome
            best_fitness = 0;
            avr_fitness = 0;
            
            for (int i=0; i<genomes.size(); i++) {
                double cur_fitness = compute_fitness_xor(X, Y, genomes[i]);
                best_fitness = max(best_fitness, cur_fitness);
                avr_fitness += cur_fitness;
            }
            avr_fitness /= max(1, (int)genomes.size()); // Avoid division by zero
            
            show_gen_stats();
            
            // Skip evolution on last generation
            if (gen == num_generations - 1) break;
            
            // Select best performing networks
            vector<Genome> selected_genomes = select_best_networks_tournament();
            
            // Create offspring for next generation
            vector<Genome> next_generation = create_next_generation(selected_genomes);
            
            // Replace current population with new generation
            genomes = next_generation;
        }
    }

    /* 
    Tournament selection: randomly selects groups of genomes and picks the best from each group
    */
    vector<Genome> select_best_networks_tournament() {
        vector<Genome> selected_genomes;
        int num_to_select = population_size;
        
        // Handle empty population case
        if (genomes.empty()) {
            cerr << "Warning: Empty population during selection" << endl;
            return selected_genomes;
        }

        for (int i=0; i<num_to_select; i++) {
            // Start with a random genome
            int best_idx = rand() % genomes.size();
            double best_fitness = genomes[best_idx].fitness;
            
            // Tournament selection
            for (int j=1; j<tournament_size; j++) {
                int idx = rand() % genomes.size();
                if (genomes[idx].fitness > best_fitness) {
                    best_idx = idx;
                    best_fitness = genomes[best_idx].fitness;
                }
            }
            
            // Add winner to selected genomes
            selected_genomes.push_back(genomes[best_idx]);
        }
        
        return selected_genomes;
    }

    /*
    Creates the next generation by crossing over parents and applying mutations
    */
    vector<Genome> create_next_generation(vector<Genome>& selected_genomes) {
        vector<Genome> next_generation_genomes;
        
        // Handle empty selected genomes
        if (selected_genomes.empty()) {
            cerr << "Warning: No genomes selected for next generation" << endl;
            create_population(initializer); // Create a fresh population
            return genomes;
        }
        
        // Get parent pairs
        vector<pair<Genome, Genome>> parent_pairs = get_parent_pairs(selected_genomes);
        
        for (int i=0; i<parent_pairs.size(); i++) {
            pair<Genome, Genome>& cur_pair = parent_pairs[i];
            Genome offspring(num_inputs, num_outputs);
            
            // Apply crossover or clone based on crossover rate
            if ((double)rand() / RAND_MAX < crossover_rate) {
                offspring = crossover_genomes(cur_pair.first, cur_pair.second);
            } else {
                // Clone the better parent
                offspring = (cur_pair.first.fitness > cur_pair.second.fitness) ? 
                            cur_pair.first : cur_pair.second;
            }
            
            // Apply mutations
            // Weight mutation (80% chance)
            if ((double)rand() / RAND_MAX < 0.8) {
                mutation_modify_weights(offspring, false);
            }
            
            // Add node mutation (5% chance)
            if ((double)rand() / RAND_MAX < 0.05) {
                cout << "SUB7" << endl; // Debug flag
                mutation_add_node(offspring, false);
            }
            
            // Add connection mutation (10% chance)
            if ((double)rand() / RAND_MAX < 0.1) {
                mutation_add_connection(offspring);
            }
            
            // Safety check - initialize if network ended up empty
            if (offspring.nodes.empty() || offspring.links.empty()) {
                initialize_first_gen_genome_fully_connected(offspring);
            }

            offspring.set_input_output_node_ids();
            next_generation_genomes.push_back(offspring);
        }
        
        // Safety check - ensure we have enough genomes
        while (next_generation_genomes.size() < population_size) {
            Genome new_genome(num_inputs, num_outputs);
            initialize_first_gen_genome_fully_connected(new_genome);
            next_generation_genomes.push_back(new_genome);
        }
        
        return next_generation_genomes;
    }
    
    /*
    Randomly pairs selected genomes for reproduction
    */
    vector<pair<Genome, Genome>> get_parent_pairs(vector<Genome>& selected_genomes) {
        vector<pair<Genome, Genome>> parent_pairs;
        int num_pairs = population_size / 2;
        
        // Handle empty or singleton selected genomes
        if (selected_genomes.size() <= 1) {
            if (selected_genomes.size() == 1) {
                // If we only have one genome, pair it with itself
                parent_pairs.push_back(make_pair(selected_genomes[0], selected_genomes[0]));
            }
            return parent_pairs;
        }

        for (int i=0; i<num_pairs; i++) {
            // Select first parent
            int parent1_idx = rand() % selected_genomes.size();
            
            // Select second parent (different from first)
            int parent2_idx = rand() % selected_genomes.size();
            int attempts = 0;
            
            // Try to get a different parent, but don't get stuck in an infinite loop
            while (parent2_idx == parent1_idx && attempts < 10) {
                parent2_idx = rand() % selected_genomes.size();
                attempts++;
            }
            
            // Add pair to parent pairs
            parent_pairs.push_back(make_pair(selected_genomes[parent1_idx], selected_genomes[parent2_idx]));
        }
        
        return parent_pairs;
    }
    
    /*
    Checks for empty networks in the population
    */
    int check_empty_networks() {
        int num_empty = 0;
        for (auto& genome: genomes) {
            if (genome.links.empty() || genome.nodes.empty()) {
                num_empty++;
            }
        }
        
        if (num_empty > 0) {
            cout << "Warning: " << num_empty << " empty network(s) detected" << endl;
        }
        
        return num_empty;
    }
    
    /*
    Generates a random weight from a Gaussian distribution
    */
    double get_random_gaussian_weight(double mean=0.0, double stddev=0.5) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }
    
    /*
    Display generation statistics
    */
    void show_gen_stats() {
        cout << "best_fitness: " << best_fitness << endl;
        cout << "average_fitness: " << avr_fitness << endl;
    }
};

// Initialize static member
int Population::next_innovation_num = 0;

#endif // POPULATION_H