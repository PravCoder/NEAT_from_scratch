
#ifndef Genome_H
#define Genome_H

/*
Represents a individual in population which is a network with nodes and links/
*/
class Genome {

    public:
        int num_inputs;
        int num_outputs;
        // nodes & connections
        vector<NodeGene> nodes:
        vector<LinkGene> links;

        vector<int> input_node_ids;
        vector<int> output_node_ids;


        // constructor takes in number of inputs/outputs
        Genome(int numInputs, int numOutputs) : num_inputs(numInputs), num_outputs(num_outputs)  {}

        void set_input_output_node_ids() {
            for (int i=0; i<nodes.size(); i++) {
                if (nodes[i].type == "input") {
                    input_node_ids.push_back(nodes[i].id);
                }
                if (nodes[i].type == "output") {
                    output_node_ids.push_back(nodes[i].id);
                }
            }
        }
        

    
}


#endif 