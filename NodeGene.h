
#ifndef Node_Gene_H
#define Node_Gene_H


struct NodeGene {
    int id;              // node-id, nodes do not have innovation numbers only links
    string type;        // input, hidden, or output
    double activation;  // final output activation value of node
    double bias;        // bias value of this specific node

    NodeGene(int node_id, string node_type) : {id(node_id), type(node_type), activation(0.0), bias(0.0)} // constructor


}



#endif 