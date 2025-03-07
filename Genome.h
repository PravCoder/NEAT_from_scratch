
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

        // constructor takes in number of inputs/outputs
        Genome(int numInputs, int numOutputs) : num_inputs(numInputs), num_outputs(num_outputs) 
        

    
}


#endif 