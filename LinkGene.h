#include <vector> 
#ifndef Link_Gene_H
#define Link_Gene_H
#define Population_H
#include "Genome.h" 
#include "NodeGene.h" 
#include "LinkGene.h" 

struct LinkGene {
    int input_node;   // source-node-id
    int output_node;  // target-node-id
    double weight;
    bool enabled;     // true or false, if false do not contribute to forward prop
    int innovation_num; // only links have innovation numbres, nodes do not

    LinkGene() : input_node(0), output_node(0), weight(0.0), enabled(true), innovation_num(0) {}  // default constructor

    LinkGene(int in_node, int output_node, double weight_val, bool enabled, int innov_num) 
        : input_node(in_node), output_node(output_node), weight(weight_val), enabled(enabled), innovation_num(innov_num) {} // constructor


    void show() {
        cout << "link-gene: " << input_node << "->" << output_node << endl;
    }
};




#endif 