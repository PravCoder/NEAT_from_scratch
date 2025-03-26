#include <vector> 
#include <unordered_map>
#ifndef Genome_H
#define Genome_H
#define Population_H
#include "Genome.h" 
#include "NodeGene.h" 
#include "LinkGene.h" 

/*
Represents a individual in population which is a network with nodes and links/
*/
class Genome {

    public:
        int num_inputs; // number of input and output nodes
        int num_outputs;
        // nodes & connections
        vector<NodeGene> nodes; // arr of node-gene-objs
        vector<LinkGene> links; // arr of link-gene-objs

        vector<int> input_node_ids; // each element is an input-node-id
        vector<int> output_node_ids; // each element is an output-node-id

        double fitness;

        unordered_map<int, LinkGene> genes_map; //  map from innovation-num to link-gene object for quick lookup


        // constructor takes in number of inputs/outputs
        Genome(int numInputs, int numOutputs) : num_inputs(numInputs), num_outputs(numOutputs)  {}

        // iterate all node-objs, if its a input-node add its id to input-node-ids same with output nodes
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

        // creates map from innov-num key to link-gene-obj for this genome, for fast lookup
        void create_innovation_num_to_link_gene_map() {
            for (int i=0; i<links.size(); i++) {
                genes_map[links[i].innovation_num] = links[i]; // innovation-num-key to link-gene-obj-value
            }
        }

        vector<int> get_input_and_hidden_nodes_indicies() {
            vector<int> input_and_hidden_nodes;
            for (int i=0; i<nodes.size(); i++) {
                if (nodes[i].type == "input" || nodes[i].type == "hidden") {
                    input_and_hidden_nodes.push_back(i);
                }
            }
            return input_and_hidden_nodes;
        }
        vector<int> get_hidden_and_output_nodes_indicies() {
            vector<int> hidden_and_output_nodes;
            for (int i=0; i<nodes.size(); i++) {
                if (nodes[i].type == "hidden" || nodes[i].type == "output") {
                    hidden_and_output_nodes.push_back(i);
                }
            }
            return hidden_and_output_nodes;
        }

        bool does_connection_exist(int source_id, int target_id) {  // note can also do by using genes-mapx
            for (int i=0; i<links.size(); i++) {
                LinkGene& cur_link = links[i];
                if (cur_link.input_node == source_id && cur_link.output_node == target_id) {
                    return true;
                }
            }
            return false;
        }

        int get_next_node_id() {
            int max_id = -100;
            for (int i=0; i<nodes.size(); i++) {
                if (nodes[i].id > max_id) {
                    max_id = nodes[i].id;
                }
            }
            max_id += 1;
            return max_id;
        }

        /* 
        Forward pass of a network is for each node need to compute its activation. To compute a activation of a node
        you get all of connections whose target-node is that node, then for each of these links multiply its weight value 
        with the activation of that links source-node, and sum all of these. Disabled links to not contirbute activation of a node.
        Each node has its own bias value that is added to its weighted sum - ignoring for now. Computes forward propgation for a single example input values so 1D vector of inputs for each input node, returns 1D vector of outputs for each output node.
        */
        vector<double> forward_propagate_single_example(vector<double> X) {
            vector<double> y_hat;

            // reset all activations to 0
            for (int i = 0; i < nodes.size(); i++) {if (nodes[i].type != "input") {nodes[i].activation = 0.0;}}
            
            // set input node activations to x-input-values using input-node-ids
            for (int i=0; i<input_node_ids.size(); i++) {
                NodeGene& cur_input_node = get_node_via_id(input_node_ids[i]);
                
                cur_input_node.activation = X[i];
                //cout << "is this input node getting set id: " <<cur_input_node.id << ", activation: " <<cur_input_node.activation << endl;
            }

            // iterate all nodes for feed-forward structure
            for (int i=0; i<nodes.size(); i++) {
                NodeGene& cur_node = nodes[i];
                if (cur_node.type == "input") {  // if its a input-node skip because its activation is already set
                    continue;
                }
                vector<LinkGene> target_links = get_links_of_target_node(cur_node.id); // get all links whose output-node is cur-node
                double weighted_sum = 0.0;  // weighted-sum of cur-node
                
                // iterate these links whose target-node is cur-node
                for (int j=0; j<target_links.size(); j++) {
                    LinkGene& cur_link = target_links[j];
                    if (cur_link.enabled == true) { // if cur-link is enabled then compute its weighted-sum by multiplying its weight and the activation of that links source-node
                        //cout << "this a weight " << cur_link.weight << ", this a act " << get_node_via_id(cur_link.input_node).activation << ", id "<<cur_link.input_node << endl;
                        weighted_sum +=  (cur_link.weight * get_node_via_id(cur_link.input_node).activation);
                    }
                }

                weighted_sum += cur_node.bias;  // add bias
                cur_node.activation = sigmoid(weighted_sum); // set activation of weighted-sum as activation of cur-node
            }

            // get output values
            for (int i=0; i<output_node_ids.size(); i++) {
                NodeGene& cur_output_node = get_node_via_id(output_node_ids[i]);
                y_hat.push_back(cur_output_node.activation);
            }

            return y_hat;
        }

        vector<LinkGene> get_links_of_target_node(int target_node_id) {  // returns all the links whose output node is given node-id
            vector<LinkGene> target_links;
            for (int i=0; i<links.size(); i++) {
                if (links[i].output_node == target_node_id) {
                    target_links.push_back(links[i]);
                }
            }
            return target_links;
        }

        NodeGene& get_node_via_id(int node_id) {   // returns node-obj given node-idx
            for (int i=0; i<nodes.size(); i++) {
                if (nodes[i].id == node_id) {
                    return nodes[i];
                }
            }
        }

        double sigmoid(double z) {
            return 1.0 / (1.0 + exp(-z));
        }

        void show() {
            cout << "genome in: " << num_inputs << " outs: " << num_outputs << endl;
            string node_str = "";
            for (int i=0; i<nodes.size(); i++) {
                node_str += to_string(nodes[i].id);
                if (nodes[i].type == "input") node_str += "-inp";
                if (nodes[i].type == "output") node_str += "-out";
                if (i < nodes.size() - 1) node_str += ", ";
                
            }
            cout << "   nodes: " << node_str << endl;

            string links_str = "";
            cout << "   links: " << endl;
            for (int i=0; i<links.size(); i++) {
                string cur_str = "";
                string is_enabled = (links[i].enabled) ? "e" : "d";
                cur_str = to_string(links[i].input_node) + "->" + to_string(links[i].output_node) + "[IN=" + to_string(links[i].innovation_num) + "]" +is_enabled;
                if (i < links.size() - 1) node_str += ", ";
                cout <<  "   " <<cur_str << endl;
            }
        }

        void show_weights() {
            create_innovation_num_to_link_gene_map();  // make sure this map is created
            cout << "   weights:" << endl;
            for (auto it = genes_map.begin(); it != genes_map.end(); ++it) {
                int innov_num = it->first;
                LinkGene& link = it->second;
                cout << "   [IN=" << innov_num << "]-"<<link.weight << endl;
            }
        }
        

    
};


#endif 