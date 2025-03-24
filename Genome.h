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

        int fitness;

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

        void get_next_node_id() {
            
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
                cur_str = to_string(links[i].input_node) + "->" + to_string(links[i].output_node) + "[IN=" + to_string(links[i].innovation_num) + "]";
                if (i < links.size() - 1) node_str += ", ";
                cout <<  "   " <<cur_str << endl;
            }
            
            
        }
        

    
};


#endif 