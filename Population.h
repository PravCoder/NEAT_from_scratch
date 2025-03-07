
#ifndef Population_H
#define Population_H

/*
Represents a collection of genomes/individuals. 
*/
class Population {
    public:
        static int next_innovation_num;
        vector<Genome> genomes;  // vector array of genom-objs for cur-population
        int population_size;
        double crossover_rate; // probability value between 0-1, for example x% of the time a crossover happens and 100-x% a clone+mutation happens. TBD: what about crossover+mutation
        int num_inputs;
        int num_outputs;

        Population(int size, int numInputs, int numOutputs, int cr) : population_size(size), crossover_rate(cr) {
            create_population();
        }
        
        // creates genome-objs for first-generation
        void create_population() {
            for (int i=0; i<population_size; i++) {
                Genome cur_genome = Genome(num_inputs, num_outputs);
                genomes.push_back(cur_genome);
                initialize_first_gen_genome_fully_connected(genomes.back()); // get the genome that we pushed to vector not the local one
            }
        }

        // only called on the first generation of genomes to create their nodes/links, called per genome in population, we have this in population because we need the next-innovation number for every link
        void initialize_first_gen_genome_fully_connected(Genome& genome) {
            // iterate number of input/output nodes and create node-gene-obj, add to this genome.ndoes
            for (int i=0; i<num_inputs; i++) {
                genome.nodes.push_back(NodeGene(i, "input"));  // pass in node-id & type
            }
            for (int i=0; i<num_outputs; i++) {
                genome.nodes.push_back(NodeGene(num_inputs+i, "output")); // output-node-id is num-inputs plus ith 
            }

            // nested loop to connect every input-node to every output-node, create/add link-objs
            for (int i=0; i<num_inputs; i++) {
                for (int j=0; j<num_outputs; j++) {
                    int input_node_id = i; 
                    int output_node_id = num_inputs + j;
                    double weight = (rand() % 200 - 100) / 100.0;
                    genome.links.push_back(LinkGene(input_node_id, output_node_id, weight, true, next_innovation_num++));
                }
            }

        }

}


int Population::next_innovation_num = 0;
