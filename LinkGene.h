
#ifndef Link_Gene_H
#define Link_Gene_H


struct LinkGene {
    int input_node;   // source-node-id
    int output_node;  // target-node-id
    double weight;
    bool enabled;
    int innovation_num; // only links have innovation numbres, nodes do not


    LinkGene(int in_node, int output_node, double weight_val, bool enabled, int innov_num) 
        : input_node(in_node), output_node(output_node), weight(weight_val), enabled(enabled), innovation_num(innov_num) {} // constructor

}




#endif 