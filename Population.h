#include <vector> 
#include <unordered_map>
#ifndef Population_H
#define Population_H
#include "Genome.h" 
#include "NodeGene.h" 
#include "LinkGene.h" 
#include <map>


/*
Represents a collection of genomes/individuals. 1 Population object is created in main and is used, cleaned, created for all generations.
*/
class Population {
    public:
        static int next_innovation_num;
        vector<Genome> genomes;  // arr of genome-objs for cur-population
        int population_size;
        double crossover_rate; // probability value between 0-1, for example x% of the time a crossover happens and 100-x% a clone+mutation happens. TBD: what about crossover+mutation
        int num_inputs;
        int num_outputs;

        // hashmap where each key is <source-node, target-node> connection value is its innovation-number, we need some link to the innovation number and its connection s->t.
        map<pair<int, int>, int> innovation_nums_map;    

        Population(int size, int numInputs, int numOutputs, double cr, string initializer) : num_inputs(numInputs), num_outputs(numOutputs), population_size(size), crossover_rate(cr) {
            create_population(initializer);
        }
        
        // creates genome-objs for first-generation
        void create_population(string initializer) {
            // iterate the population-size
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
            
            // if given connection exists in our map, return its innovation number value
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
                    double weight = (rand() % 200 - 100) / 100.0;
                    int innov_num =  get_innovation_number(input_node_id, output_node_id);
                    genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, innov_num));
                }
            }

        }

        // given empty-genome-obj initializes it by creating its node and 
        void initialize_first_gen_genome_randomly_connected(Genome& genome, double connection_prob = 0.5) {
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
                        int weight = (rand() % 200 - 100) / 100.0;
                        int innov_num =  get_innovation_number(input_node_id, output_node_id);
                        genome.links.push_back(LinkGene(input_node_id,output_node_id,weight,true, innov_num));
                    }
                }
            }

        }

        void show_pop() {
            cout << "Population size: " << population_size << endl;
            for (int i=0; i<population_size; i++) {
                genomes[i].show();
            }
        }

        void crossover_genomes(Genome& parent1, Genome& parent2) {
            Genome& better_parent = (parent1.fitness > parent2.fitness) ? parent1 : parent2;
            Genome& lesser_parent = (parent1.fitness > parent2.fitness) ? parent1 : parent2;
            // init offspring genome-obj
            Genome offspring = Genome(better_parent.num_inputs, better_parent.num_inputs);
            // inherit all nodes from better parent (there are other approaches)
            for (int i=0; i<better_parent.nodes.size(); i++) {
                offspring.nodes.push_back(better_parent.nodes[i]);
            }
            // create-map from innov-num to link-gene of lesser parent for quick lookup
            lesser_parent.create_innovation_num_to_link_gene_map();
            
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
            offspring.set_input_output_node_ids();
            
        }

};


int Population::next_innovation_num = 0; // static variable thats local to population

#endif 