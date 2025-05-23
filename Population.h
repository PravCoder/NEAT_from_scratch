#include <vector> 
#include <unordered_map>
#ifndef Population_H
#define Population_H
#include "Genome.h" 
#include "NodeGene.h" 
#include "LinkGene.h" 
#include <map>
#include <algorithm> 
#include <random>  


/*
Represents a collection of genomes/individuals. 1 Population object is created in main and is used, cleaned, created for all generations.
*/
class Population {
    public:
        static int next_innovation_num; // the current latest innovation number, increment to get next IN
        vector<Genome> genomes;  // arr of genome-objs for cur-population
        int population_size;
        double crossover_rate; // probability value between 0-1, for example x% of the time a crossover happens and 100-x% a clone+mutation happens. TBD: what about crossover+mutation
        int num_inputs;
        int num_outputs;
        string initializer;

        // hashmap where each key is <source-node, target-node> connection value is its innovation-number, we need some link to the innovation number and its connection s->t.
        map<pair<int, int>, int> innovation_nums_map;   
        
        int num_generations;
        double best_fitness;
        double avr_fitness;
        int tournament_size;

        int num_weight_mutations = 0;
        int num_add_node_mutations = 0;
        int num_add_connection_mutations = 0;  // more gen stats TBD


        Population(int size, int numInputs, int numOutputs, double cr, string initializer, int num_generations, int tournament_size) : num_inputs(numInputs), num_outputs(numOutputs), population_size(size), crossover_rate(cr), initializer(initializer), num_generations(num_generations), tournament_size(tournament_size) {
            // create_population(initializer); // initial population is created here
        }
        
        // creates genome-objs for initial-first-generation
        void create_population(string initializer) {
            // iterate the population-size
            cout << "init pop-size: " << population_size << endl;
            for (int i=0; i<population_size; i++) {
                Genome cur_genome = Genome(num_inputs, num_outputs);  // create genome-obj with num-inputs-outputs
                genomes.push_back(cur_genome);                        // add genome-obj to genomes-arr
                if (initializer == "fully_connected") {
                    initialize_first_gen_genome_fully_connected(genomes.back()); // get the genome that we pushed to vector not the local one
                }
                if (initializer == "rand_connected") {
                    initialize_first_gen_genome_randomly_connected(genomes.back());
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
        // only called on the first generation of genomes to create their nodes/links, called per genome in population, we have this in population because we need the next-innovation number for every link
        void initialize_first_gen_genome_fully_connected(Genome& genome) {
            // srand(time(NULL)); // put this everytime you want to do something random
            // iterate number of input/output nodes and create node-gene-obj, add to this genome.ndoes
            for (int i=0; i<num_inputs; i++) {
                genome.nodes.push_back(NodeGene(i, "input"));  // pass in node-id & type
            }
            for (int i=0; i<num_outputs; i++) {
                genome.nodes.push_back(NodeGene(num_inputs+i, "output")); // output-node-id is num-inputs plus ith 
            }
            genome.set_input_output_node_ids();

            // nested loop to connect every input-node to every output-node, create/add link-objs
            for (int i=0; i<num_inputs; i++) {
                for (int j=0; j<num_outputs; j++) {
                    int input_node_id = i; 
                    int output_node_id = num_inputs + j; // starts from last input-node-id
                    double weight = get_random_uniform_weight();
                    int innov_num =  get_innovation_number(input_node_id, output_node_id);
                    genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
                }
            }

        }

        // given empty-genome-obj initializes it by creating its node and 
        void initialize_first_gen_genome_randomly_connected(Genome& genome, double connection_prob = 0.7) {
            // srand(time(NULL));
            // iterate number of input/output nodes and create node-gene-obj, add to this genome.ndoes
            for (int i=0; i<num_inputs; i++) {
                genome.nodes.push_back(NodeGene(i, "input"));  // pass in node-id & type
            }
            for (int i=0; i<num_outputs; i++) {
                genome.nodes.push_back(NodeGene(num_inputs+i, "output")); // output-node-id is num-inputs plus ith 
            }
            
            genome.set_input_output_node_ids();

            // iterate every input-node-id, iterate every output-node-id
            for (int i=0; i<num_inputs; i++) {
                for (int j=0; j<num_outputs; j++) {
                    if ((double)rand() / RAND_MAX < connection_prob) {
                        int input_node_id = i;
                        int output_node_id = num_inputs+j;
                        double weight = get_random_uniform_weight();
                        int innov_num =  get_innovation_number(input_node_id, output_node_id);
                        genome.links.push_back(LinkGene(input_node_id,output_node_id,weight,true, innov_num));
                    }
                }
            }

            // make sure each output node has at least one connection
            for (int j=0; j<num_outputs; j++) {  // iterate all output nodes
                int output_node_id = num_inputs+j; // get cur-output-node-id
                bool has_connection = false;   // init cur-output node has connection to false
                
                // iterate all links of the genome if there exists 1 link with output node of cur-output node then it has connection
                for (auto& link : genome.links) {
                    if (link.output_node == output_node_id) {
                        has_connection = true;
                        break;
                    }
                }
                
                // if cur-output-node doesnt have a connection to it, create a random connection to cur-output-node
                if (has_connection == false) {
                    int input_node_id = genome.input_node_ids[rand() % genome.input_node_ids.size()];  // get a randomt input-node-id
                    double weight = get_random_uniform_weight();  // get random weight
                    int innov_num = get_innovation_number(input_node_id, output_node_id); // get next innovation number for this new connection
                    // add this new link with  random-input-node-id and cur-output-node
                    genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
                }
            }

        }

        void show_pop() {
            cout << "Population size: " << population_size << endl;
            for (int i=0; i<population_size; i++) {
                genomes[i].show();
            }
        }

        Genome crossover_genomes(Genome& parent1, Genome& parent2) {
            Genome og_parent1 = parent1;  // save the parents to check if this crossover produces a empty-network
            Genome og_parent2 = parent2;

            // choose the better parent, if fitness equal choose randomly
            Genome* better_parent_ptr;  // use pointers instead of references so we can assign them later, 
            Genome* lesser_parent_ptr;
            if (parent1.fitness > parent2.fitness) {
                better_parent_ptr = &parent1;
                lesser_parent_ptr = &parent2;
            } else if (parent2.fitness > parent1.fitness) {
                better_parent_ptr = &parent2;
                lesser_parent_ptr = &parent1;
            } else {
                if (rand() % 2 == 0) {
                    better_parent_ptr = &parent1;
                    lesser_parent_ptr = &parent2;
                } else {
                    better_parent_ptr = &parent2;
                    lesser_parent_ptr = &parent1;  // set pointer equal address of object
                }
            }
            // use dereferenced pointers to access genome-objects
            Genome& better_parent = *better_parent_ptr;
            Genome& lesser_parent = *lesser_parent_ptr;

            // init offspring genome-obj
            Genome offspring = Genome(better_parent.num_inputs, better_parent.num_outputs);
            // inherit all nodes from better parent *(there are other approaches)*
            for (int i=0; i<better_parent.nodes.size(); i++) {
                offspring.nodes.push_back(better_parent.nodes[i]);
            }

            // create-map from innov-num to link-gene of lesser parent for quick lookup
            // check if the lesser parent is not empty and the creation of its genes map is empty
            lesser_parent.create_innovation_num_to_link_gene_map();
            if (lesser_parent.genes_map.empty() &&  lesser_parent.is_empty() == false) {
                cerr << "-genes map empty in crossover" << endl;
            }
            
            for (int i=0; i<better_parent.links.size(); i++) {
                LinkGene& better_link = better_parent.links[i];

                // if innov-num of current link exists in the other lesser parent, then it is a matching gene
                if (lesser_parent.genes_map.find(better_link.innovation_num) != lesser_parent.genes_map.end()) {
                    // matching-gene randomly choose to inherit from either parent
                    if (rand() % 2 == 0) { // inherit cur-link from better-parent
                        offspring.links.push_back(better_link);
                    } else {  
                        offspring.links.push_back(lesser_parent.genes_map[better_link.innovation_num]);  // inherit cur-link from lesser-parent
                    }
                } else {  // if innov-num of current link does not exist in other lesser parent, then it is a disjoint or excess link
                    offspring.links.push_back(better_link);
                }

            }

            // if (offspring.links.size() == 0) {
            //     offspring = better_parent;
            // }
            
            // check if crossover produces a empty-network offspring, when neither parents were empty
            if ((og_parent1.is_empty() == false && og_parent2.is_empty() == false) && offspring.is_empty() == true) {
                cerr << "empty network created by crossover" << endl;
            }
            return offspring;
        }

        void mutation_add_connection(Genome& offspring) {
            Genome og_offspring = offspring;  // save original offspring to check if it was empty
            // if by change we recived a empty network, fill it up
            if (offspring.is_empty()) {
                cerr << "empty network given to mutation - add connection" << endl;
                initialize_first_gen_genome_randomly_connected(offspring);
                offspring.set_input_output_node_ids();
                return;
            }
            // cout << "----Mutation Add Connection---:" << endl;
            // srand(time(NULL));
            int num_hidden = offspring.nodes.size() - (num_inputs + num_outputs);  // compute number of hidden nodes
            int num_connections_possible = (num_inputs*num_outputs) + (num_inputs*num_hidden) + (num_hidden*num_outputs); // input->output, input->hidden, hidden->out
            num_connections_possible += (num_hidden * (num_hidden - 1)) / 2;   // each hidden node can connect to all hidden nodes with higher indices
            num_connections_possible -= offspring.links.size();   // minus the number of connections that already exist
            
            const int MAX_ATTEMPTS = num_connections_possible * 5;
            // const int MAX_ATTEMPTS = 10;
            // cout << "max_attempts: " << MAX_ATTEMPTS << endl;
            
            // compile all of the input & hidden nodes because those are valid nodes for source-node
            vector<int> input_hidden_node_indicies = offspring.get_input_and_hidden_nodes_indicies();
            // compile all of the hidden & output nodes because those are valid nodes for target-node
            vector<int> hidden_output_node_indicies = offspring.get_hidden_and_output_nodes_indicies();

            for (int attempt=0; attempt<MAX_ATTEMPTS; attempt++) {
                
                int source_indx = input_hidden_node_indicies[rand() % input_hidden_node_indicies.size()];  // randomlly choose input/hidden-node indx for target
                int target_indx = hidden_output_node_indicies[rand() % hidden_output_node_indicies.size()];   // randomlly choose hidden/output-node indx for target

                NodeGene& source_node = offspring.nodes[source_indx];
                NodeGene& target_node = offspring.nodes[target_indx];

                // when trying to connect hidden->hidden if sources-id is more than targets-id its a cycle go to next attempt
                if (source_node.type == "hidden" && target_node.type == "hidden" && source_node.id >= target_node.id) {
                    continue;
                }

                // check if connection already exists, based on node-obj-ids deteremine if connection exists
                if (offspring.does_connection_exist(source_node.id, target_node.id) == true) {
                    continue;
                }


                // check if this mutation created a empty network
                if ((og_offspring.is_empty() == false) && (offspring.is_empty() == true) ) {
                    cerr << "empty network created by mutation - add connection" << endl;
                }

                // create new connection mutation
                int new_innovation_num = get_innovation_number(source_node.id, target_node.id);
                double new_weight = get_random_uniform_weight();
                offspring.links.push_back(LinkGene(source_node.id, target_node.id, new_weight, true, new_innovation_num));
                return;

            }
        }

        void mutation_add_node(Genome& offspring, bool show_info) {
            Genome og_offspring = offspring; 
            // if by change we recived a empty network, fill it up
            if (offspring.is_empty()) {
                cerr << "empty network given to mutation - add node" << endl;
                initialize_first_gen_genome_randomly_connected(offspring);
                offspring.set_input_output_node_ids();
                return;
            }
            // choose a random existing connection
            int rand_link_index = rand() % offspring.links.size();
            LinkGene& link_to_split = offspring.links[rand_link_index];    // reference, modified here is modified in offspring.links
            // disable random existing link
            link_to_split.enabled = false;
            
            // create new-node-obj passing in new id and its always a hidden-node
            NodeGene new_node = NodeGene(offspring.get_next_node_id(), "hidden");

            // create new connection from original-source-node to new-node, weight set to 1, passing in new innovation number
            LinkGene link_1 = LinkGene(link_to_split.input_node, new_node.id, 1, true, get_innovation_number(link_to_split.input_node, new_node.id));

            // create new connection from new-node to original-target-node, weight same as link that was split, new innovation number
            LinkGene link_2 = LinkGene(new_node.id, link_to_split.output_node, link_to_split.weight, true, get_innovation_number(new_node.id, link_to_split.output_node));

            offspring.nodes.push_back(new_node);  // add new node-obj, all other nodes still exist
            offspring.links.push_back(link_1);   // add 2 newly created links
            offspring.links.push_back(link_2);


            if (show_info) {
                cout << "link_to_split: ";
                link_to_split.show();
                cout << "new-node: " << new_node.id << endl;
            }

            // check if this mutation created a empty network
            if ((og_offspring.is_empty() == false) && (offspring.is_empty() == true) ) {
                cerr << "empty network created by mutation - add node" << endl;
            }
            
            // cout << "after mutation: " << endl;
            // offspring.show();

        }

        void mutation_modify_weights(Genome& offspring, bool show_info) {
            Genome og_offspring = offspring; 
            // if by change we recived a empty network, fill it up
            if (offspring.is_empty()) {
                cerr << "empty network given to mutation - weight mutate" << endl;
                initialize_first_gen_genome_randomly_connected(offspring);
                offspring.set_input_output_node_ids();
                return;
            }

            for (int i=0; i<offspring.links.size(); i++) {
                double before_weight = offspring.links[i].weight;
                double rand_weight = get_random_uniform_weight();
                if (show_info) {
                    cout << "before weight: " <<before_weight<< ", after weight: " << offspring.links[i].weight + rand_weight << endl;
                }
                offspring.links[i].weight = offspring.links[i].weight + rand_weight;
                offspring.links[i].weight = std::max(-2.0, std::min(2.0, offspring.links[i].weight)); // map teh weight between range
            }

            // check if this mutation created a empty network
            if ((og_offspring.is_empty() == false) && (offspring.is_empty() == true) ) {
                cerr << "empty network created by mutation - mutate weights" << endl;
            }
            // cout << "after: " << endl;
            // offspring.show();
        }

        /*
        Given X which is all the examples in your dataset, and Y which all the labels for your each example in X, and an genome-obj.
        Goes through each example and feed forwards into the genomes network and gets its predictions and uses the labels in a fitness equation to compute that genomes fitness.
        Higher the fitness the better genome is performing.
        */
        double compute_fitness_xor(vector<vector<double>> X, vector<vector<double>> Y, Genome& genome) {
            if (genome.is_empty() || genome.are_all_links_disabled())  { // make sure empty and disabled networks have very low fitness so their genes dont get passed on, and empty/disabled networks appear in the population again
                return 0.0001;  // penalize empty networks even if they are created by accident
            }

            double total_error = 0.0;
            
            for (int i=0; i<X.size(); i++) {   // iterate every example
                vector<double> cur_example = X[i];  // get cur-example inputs
                vector<double> prediction_cur_example = genome.forward_propagate_single_example(cur_example); // pass cur-example-inputs into network get predictions vector for output nodes
                
                double example_error = 0.0;  // error of current example
                for (int j=0; j<prediction_cur_example.size(); j++) {          // iterate all predictions so for each output node
                    double error = prediction_cur_example[j] - Y[i][j];            // compute error of current output node, add to total-example-error
                    example_error += error*error; 
                }

                total_error += example_error;  // add total-example-error to total error after computing error of cur-example
            }

            double fitness = 1.0 / (1.0 + total_error); // convert error across all examples into fitness value
            genome.fitness = fitness;
            return fitness;
        }

        /*
        Loop to compute fitness and create next generation of genomes based on best performing genoms of previous population.
        */
        void evolutionary_loop(vector<vector<double>> X, vector<vector<double>> Y) {
            // create_population(initializer);  // if its the first generation create initial population
            for (int i=0; i<num_generations; i++) {
                cout << "-----Generation #" << i << "-----" << endl;
                cout << "empty: " << check_empty_networks() << endl;
                cout << "disabled: " << check_disabled_networks() << endl;
                cout << "floating: " << check_floating_output_networks() << endl;
                cout << "genomes.size() = " << genomes.size() << ", pop_size = " << population_size << endl;

                // compute fitness of population
                best_fitness = 0, avr_fitness = 0;   // reset gen-stats after every generation
                
                for (int i=0; i<genomes.size(); i++) {  // iterate all genomes compute its fitness using entire dataset and labels, update fitness states
                    double cur_fitness = compute_fitness_xor(X, Y, genomes[i]);
                    best_fitness = max(best_fitness, cur_fitness);
                    avr_fitness += cur_fitness;
                }
                avr_fitness /= genomes.size();
                show_gen_stats();  
                // show_pop();
                
                

                // select best performing networks using some method - func
                vector<Genome> selected_genomes = select_best_networks_tournament();  // these are the networks we pair for reproduction
                cout << "selected_genomes: " << selected_genomes.size() << endl;
                if (check_if_vector_has_empty_networks(selected_genomes) == true) {
                    cerr << "selected genomes has empty networks"<< endl;
                }

                // create offsprings by pairing these best networks crossing them over, mutate these offsprings randomlly - create_next_generation()
                vector<Genome> next_generation_genomes = create_next_generation(selected_genomes);
                cout << "next_gen_genomes: " << next_generation_genomes.size() << endl;
                if (check_if_vector_has_empty_networks(next_generation_genomes) == true) {
                    cerr << "next generation genomes has empty networks" << endl;
                }
                
                check_empty_networks(); // is never printed for some reason
                genomes.clear();
                // replace genomes with next_generation_genomes;
                genomes = next_generation_genomes;
                population_size = genomes.size();  // update population size
            }
        }

        /*
        Given number of genomes to select for reproduction, for each of these genomes create a randomly select x-tournament-size
        genomes and among them select the genome with best fitness. For number of genomes of next generation randomlly select a group of genomes 
        and add the genome with highest fitness to next population.
        Promotes diversity through random sampling of groups. Promotes best networks by selecting highest fitness in each random group.
        */
        vector<Genome> select_best_networks_tournament() {
            check_empty_networks(); // check how many empty networks are in genomes before selecting best ones
            // check if the population-variable size is more thatn the genomes size, this is where empty networks originate if this condition is true
            if (population_size > genomes.size()) {
                cout << "Debug: genomes.size() = " << genomes.size() << ", population_size = " << population_size << endl;
            }
            // cout << "Debug-2: genomes.size() = " << genomes.size() << ", population_size = " << population_size << endl;
            
            vector<Genome> selected_genomes;

            // number of genomes to select for reproduction, these genomes have to produce enough offsprings for next gen
            int num_to_select = population_size;  

            for (int i=0; i<num_to_select; i++) {
                int best_indx = rand() % population_size;
                double cur_group_best_fitness = genomes[best_indx].fitness;
                
                // create a group for cur-genome we need to select
                for (int j=1; j<tournament_size; j++) {
                    int indx = rand() % population_size; // random indx to select random genome for this group
                    if (genomes[indx].fitness > cur_group_best_fitness) {  // if cur-random-genome-selected-for-this-group 
                        best_indx = indx;
                        cur_group_best_fitness = genomes[best_indx].fitness;
                    }
                }
                // add the best genome with highest to selected genomes for next gen reporduction
                // if (genomes[best_indx].is_empty()) {
                //     cout << "added empty in select" << endl;
                // }
                selected_genomes.push_back(genomes[best_indx]);
            }
            return selected_genomes;
        }

        /*
        
        */
        vector<Genome> create_next_generation(vector<Genome>& selected_genomes) {
            vector<Genome> next_generation_genomes;
            // create offpsrings by pairing selected networks
            vector<pair<Genome, Genome>> parent_pairs = get_parent_pairs(selected_genomes);
            // iterate all parent pairs that we selected for reproduction, for each create 2 new offsprings
            cout << "parent pairs=" << parent_pairs.size() << endl;
            for (int i=0; i<parent_pairs.size(); i++) {
                pair<Genome, Genome>& cur_pair = parent_pairs[i];
                Genome cur_offspring(num_inputs, num_outputs);
                // apply crossover rate, if crossvoer create offsprings from cur-pair-parents, else clone genome with higher fitness
                if ((double)rand() / RAND_MAX < crossover_rate) {
                    cur_offspring = crossover_genomes(cur_pair.first, cur_pair.second);
                } else {
                    cur_offspring = (cur_pair.first.fitness > cur_pair.second.fitness) ? cur_pair.first : cur_pair.second;
                }
                // randomly choose which mutation to do after crossing over or cloning
                if ((double)rand() / RAND_MAX < 0.8) {
                    mutation_modify_weights(cur_offspring, false); // should modify reference
                }
                if ((double)rand() / RAND_MAX < 0.05) {
                    // cout << "SUB7" << endl;
                    mutation_add_node(cur_offspring, false);
                }
                if ((double)rand() / RAND_MAX < 0.1) {
                    mutation_add_connection(cur_offspring);
                }

                if (cur_offspring.nodes.empty() || cur_offspring.links.empty()) {
                    initialize_first_gen_genome_fully_connected(cur_offspring);
                }

                cur_offspring.set_input_output_node_ids();
                next_generation_genomes.push_back(cur_offspring);  // add offspring to new generation
                // cout << "cur off: " << cur_pair.first.fitness << ", " << cur_pair.second.fitness << endl;


                // Create SECOND offspring (swap parents for more diversity)
                Genome offspring2(num_inputs, num_outputs);
                if ((double)rand() / RAND_MAX < crossover_rate) {
                    offspring2 = crossover_genomes(cur_pair.second, cur_pair.first); // Swap parents
                } else {
                    offspring2 = (cur_pair.second.fitness > cur_pair.first.fitness) ? cur_pair.second : cur_pair.first;
                }
                
                // Apply mutations to second offspring
                if ((double)rand() / RAND_MAX < 0.8) {
                    mutation_modify_weights(offspring2, false);
                }
                if ((double)rand() / RAND_MAX < 0.05) {
                    mutation_add_node(offspring2, false);
                }
                if ((double)rand() / RAND_MAX < 0.1) {
                    mutation_add_connection(offspring2);
                }
                offspring2.set_input_output_node_ids();
                next_generation_genomes.push_back(offspring2);
            }
            return next_generation_genomes;

        }
    
        vector<pair<Genome, Genome>> get_parent_pairs(vector<Genome>& selected_genomes) {  
            vector<pair<Genome, Genome>> parent_pairs; // just randomly pairs the selected-genomes.
            int num_pairs = population_size / 2;

            // for every pair we need to randomly select 2 genomes
            for (int i=0; i<num_pairs; i++) {
                int parent1_indx = rand() % selected_genomes.size();  //  select first parent

                int parent2_indx = rand() % selected_genomes.size();  // while second parent is  equal to first keep randomly select it
                while (parent2_indx == parent1_indx) {
                    parent2_indx = rand() % selected_genomes.size();
                }

                // Genome parent1 = selected_genomes[parent1_indx];
                // Genome parent2 = selected_genomes[parent2_indx];
                // parent_pairs.push_back(make_pair(parent1, parent2));  // add pair
                parent_pairs.push_back(make_pair(selected_genomes[parent1_indx], selected_genomes[parent2_indx])); 
            }
            return parent_pairs;
        }

        int check_empty_networks() {
            int num_empty = 0;
            for (auto genome: genomes) {
                if (genome.is_empty() == true) {
                    num_empty += 1;
                }
            }
            if (num_empty > 0) {
                cerr << "has empty-networks: " << num_empty << endl;
            }
            return num_empty;
        }

        int check_disabled_networks() {
            int num_disabled = 0;
            for (auto genome: genomes) {
                if (genome.are_all_links_disabled() == true) {
                    num_disabled += 1;
                }
            }
            return num_disabled;
        }

        int check_floating_output_networks() {
            int count = 0;
            for (auto genome: genomes) {
                if (genome.has_floating_outputs() == true) { // if cur genome has a floating output node meaning it has a output node that doesn't have any connections
                    count +=1 ;
                }
            }
            return count;
        }

        bool check_if_vector_has_empty_networks(vector<Genome> arr) {
            for (auto genome: arr) {
                if (genome.is_empty() == true) {
                    return true;
                }
            }
            return false;
        }

        Genome& get_best_network() {
            double cur_max_fitness = -1; 
            int best_idx = 0;
        
            for (int i=0; i<genomes.size(); i++) {
                if (genomes[i].fitness > cur_max_fitness) {
                    cur_max_fitness = genomes[i].fitness;
                    best_idx = i;
                }
            }
            return genomes[best_idx];
        }

        double get_random_gaussian_weight(double mean=0.0, double stddev=0.2) {
            static std::random_device rd;  // Seed
            static std::mt19937 gen(rd()); // Mersenne Twister random number generator
            std::normal_distribution<double> dist(mean, stddev); // Gaussian distribution
            return dist(gen);
        }
        double get_random_uniform_weight() {
            static std::random_device rd;  // Seed
            static std::mt19937 gen(rd()); // Mersenne Twister random number generator
            std::uniform_real_distribution<double> dist(-1.0, 1.0); // Uniform distribution between -1 and 1
            return dist(gen);
        }

        void show_gen_stats() {
            cout << "best_fitness: " << best_fitness << endl;
            cout << "average_fitness: " << avr_fitness << endl;
        }

};


int Population::next_innovation_num = 0; // static variable thats local to population

#endif 