import numpy as np
import re

"""
Representation of a genome
self.innovation_nums = {innovation-num: "source->target", ...}
self.weights = {innovation-num: weight-value}
self.connections = {3: ['1_IN1'], 4: ['2_IN3'], 6: ['5_IN4'], 7: ['2_IN5']}, {node-id": [list of connections sourceNode_IN[innovation-num] ]}
"""
class NeatNeuralNetwork: 

    def __init__(self, innovation_nums, input_nodes, output_nodes, bias_node_id, weights={}, seed_individual=False, initializer=None, fitness=0):  # representation of a genome
        self.innovation_nums = innovation_nums
        self.input_nodes = input_nodes  # ids of input/output nodes
        self.output_nodes = output_nodes
        self.all_nodes = [] # ids of all nodes in network including input/output/bias-node
        self.weights = weights  # {innovation-num: weight value of that connecion}
        self.bias_node_id = bias_node_id
        self.seed_individual = seed_individual
        self.initializer = initializer
        if self.seed_individual == True: self.initialize_weights() # only init weights if it is first generation genome, else it already inherited weights during crossover
        # {{target: [source-ids]}, {'3': ['1_IN1']}, has the from-source node and innovation number of that connection in the value of each target node key. Contains hiddne/output nodes who have some connection to them, not nodes that dont have any connections to them
        self.connections = {} 
        self.max_IN = self.update_max_IN()
        self.Z = {}  # {node-id: weight-sum-scalar}
        self.A = {}  # {node-id: activated-sum}
        self.fitness = fitness
        self.prepare_network()


    def initialize_weights(self):
        num_weights = len(self.innovation_nums.keys())
        if self.initializer == "normal":
            distribution = np.random.randn(num_weights)
        if self.initializer == "glorot_normal":
            std = np.sqrt(2 / (len(self.input_nodes) + len(self.output_nodes)))
            distribution = np.random.randn(num_weights) * std
        if self.initializer == "glorot_uniform":
            limit = np.sqrt(6 / (len(self.input_nodes) + len(self.output_nodes)))  # Glorot uniform initialization
            distribution = np.random.uniform(-limit, limit, num_weights)
        if self.initializer == "he_normal":
            std = np.sqrt(2 / len(self.input_nodes))  # He normal initialization
            distribution = np.random.randn(num_weights) * std
        if self.initializer == "he_uniform":
            limit = np.sqrt(6 / len(self.input_nodes))  # He uniform initialization
            distribution = np.random.uniform(-limit, limit, num_weights)

        for i, in_num in enumerate(list(self.innovation_nums.keys())):
            self.weights[in_num] = distribution[i]

        print(f"Initalized weights: {self.weights}\n")
        return self.weights
    
    """
    For each node in network creates connections dict, where each key is node-id and its value is list of the from-source nodes connected to it
    and that corresponding target node source node IN number so we can look up the weight value efficently
    Connections: {3: ['1_IN1'], 4: ['2_IN3'], 6: ['5_IN4'], 7: ['2_IN5']}, each target node can have multiple source-strs
    disabled connections like 2->4D in innovation_nums wont show up here. 
    """
    def prepare_network(self):
        for in_num, connection_str in list(self.innovation_nums.items()):

            if "D" in connection_str: # omit disabled connection, we they still remain in the genome innovation list for possiblity for reenabling
                continue

            source, target =  int(connection_str.split("->")[0]), int(connection_str.split("->")[1])
            if source not in self.all_nodes: self.all_nodes.append(int(source))
            if target not in self.all_nodes: self.all_nodes.append(int(target))

            source_str = f"{source}_IN{in_num}"
            if int(target) not in list(self.connections.keys()):
                self.connections[int(target)] = [source_str]
            elif int(target) in list(self.connections.keys()):
                self.connections[int(target)].append(source_str)


        print(f"Connections: {self.connections}\n")  # contains only hidden/output nodes
        self.all_nodes = self.get_topological_order()
        print(f"all_nodes: {self.all_nodes}")
        return self.connections
    
    def update_max_IN(self):
        self.max_IN = max(list(self.innovation_nums.keys()))
        return self.max_IN
    def get_max_node_id(self):
        max_node_id = 0
        for key, value in self.innovation_nums.items():
            if "D" not in value:
                source, target = map(int, value.split("->"))
                max_node_id = max(max_node_id, source, target)
        return max_node_id
    
    def relu(self, Z):
        return np.maximum(0, Z)
    
    def sigmoid(self, Z):
        Z = np.clip(Z, -500, 500)  
        return 1 / (1 + np.exp(-Z))
    
    def softmax(self, Z):
        exp_z = np.exp(Z - np.max(Z))  # sub max(z) for numerical stability
        return exp_z / np.sum(exp_z)
    
    """
    Topological sort the ndoes in our DAG graph, because to compute activation of cur-node need acitvation of its source-node, if we havent computed 
    activation of source-node we cant compute activation of cur-node. 
    """
    def get_topological_order(self):
        dependencies = {node: set() for node in self.all_nodes}
        
        for target, source_list in self.connections.items():
            for source_str in source_list:
                source = int(source_str.split('_')[0]) 
                dependencies[target].add(source)
        
        ordered_nodes = self.input_nodes.copy()
        processed = set(self.input_nodes)
        
        if self.bias_node_id != -1:
            ordered_nodes.append(self.bias_node_id)
            processed.add(self.bias_node_id)
        
        while len(processed) < len(self.all_nodes):
            for node in self.all_nodes:
                if node not in processed:
                    if all(dep in processed for dep in dependencies[node]):
                        ordered_nodes.append(node)
                        processed.add(node)
        
        return ordered_nodes
    
    def extract_connection_str(self, connec_str): # value of innovation-num
        source, target =  int(connec_str.split("->")[0]), int(connec_str.split("->")[1])
        return source, target
    
    def forward_propagation(self, X): # [x1, x2, x3,..] every element of x in input node value

        # topologicaly sorted node-ids to prevent trying to acess activation of a source node to compute cur-node acitvation, when we havent computed activation of that source-node yet
        top_sorted_nodes = self.all_nodes
        print(f"all_nodes: {self.all_nodes}")
        print(f"All_nodes_topological: {top_sorted_nodes}")

        for cur_node in top_sorted_nodes:   # iterate every node
            anomalies = [] # bias nodes & nodes that dont have a connection to them, want to exlcude them in below Z,A computations because they have a predefined output
            # if its not a bias target and doesnt have any connections its output is 0
            if cur_node not in list(self.connections.keys()) and cur_node not in self.input_nodes and cur_node != self.bias_node_id: # if cur-node doesnt have any connections to it, then it outputs 0
                print(f"cur-node doesnt have connections: {cur_node}")
                self.Z[cur_node] = 0
                self.A[cur_node] = self.relu(self.Z[cur_node])
                anomalies.append(cur_node)
            if cur_node == self.bias_node_id: # handle activation/Z of bias node it self (its connections are done above)
                self.Z[cur_node] = 1
                self.A[cur_node] = 1
                anomalies.append(cur_node)
            print(f"anomalies: {anomalies}")

            if cur_node not in self.input_nodes and cur_node != self.bias_node_id:  # if it is not a input node compute forward  and it doesnt have any connections to it (its out will be zero)
                print(f"Cur-node: {cur_node}")
                if cur_node in self.connections: 
                    source_list = self.connections[cur_node] # get list of connections for cur-node only if it has connections
                else:
                    continue
                for source_str in source_list:   # iterate every source-str "1_1N2" for cur-node, every connection of cur-node
                    pattern = r"(\d+)_IN(\d+)"
                    match = re.match(pattern, source_str)
                    source_node = int(match.group(1))  
                    print(f"Source-node: {source_node}") # the cur-nodes sources
                    print(f"A={self.A}, Z={self.Z}")
                    innovation_number = int(match.group(2)) 

                    # if cur-node is not bias node and is not a node that doesnt have any connections to it
                    if cur_node not in anomalies:
                        # if the source-node of cur-node is a input-node
                        if source_node in self.input_nodes:
                            indx = self.input_nodes.index(source_node)  # get input-node indx of this source-node
                            self.Z[cur_node] = X[indx] * self.weights[innovation_number]
                            self.A[cur_node] = self.relu(self.Z[cur_node])
                        else:  # if the source-node of cur-node is not a input-node, either hidden or output
                            if cur_node in self.output_nodes:  # its output-node
                                self.Z[cur_node] = self.A[source_node] * self.weights[innovation_number]  # activation-source-node * weight connecting source to cur-node
                                #self.A[cur_node] = self.softmax(self.Z[cur_node]) * self.weights[innovation_number]
                            else:       # its hidden node
                                print(self.weights[innovation_number])
                                self.Z[cur_node] = self.A[source_node] * self.weights[innovation_number]
                                self.A[cur_node] = self.relu(self.Z[cur_node]) * self.weights[innovation_number]
    
        # handle activation of output nodes for softmax
        output_Z_vector = [] # each element is weighted sum of that output-node
        for cur_node in self.output_nodes:   # iterate each output-node in order
            output_Z_vector.append(self.Z[cur_node])  # add cur-output-node-z-value to vector in order
                
        output_A_vector = self.softmax(output_Z_vector)   # returns activations for each Z-output-node in order     
        for i, out in enumerate(self.output_nodes): # use index of order in which is appeared in output-nodes (order in which we added to output-z-vector)
            self.A[out] = output_A_vector[i]           # to set the otuput-node-activation to softmax-act-val
        print(f"output_A_vector: {output_A_vector}\n")
        #print(f"self.Z: {self.Z}")
        print(f"self.A: {self.A}, consists of only hidden/output/bias\n")
        # iterate bias connections

def main():
    input_nodes = [1, 2]
    output_nodes = [6, 7]  # order of output-nodes in output_A_vector

    # each connection-str it is source-node-id->target-node-id # include bias node here can have multiple connections to different nodes, IN: source-target
    IN = {1:"1->3", 2:"1->4", 4:"5->6" , 5:"2->7", 3:"2->4", 6: "8->5"} 
    
    n1 = NeatNeuralNetwork(innovation_nums=IN, input_nodes=input_nodes,output_nodes=output_nodes, bias_node_id=8, seed_individual=True, initializer="glorot_normal")
    n1.forward_propagation([1, 2])

#main()  # uncomment when importating from other file to avoid run

# TBD how is topology of network initalized, for initial population
# DONE: add bias node connection after computing Z
# TBD: check forward using calculator
# TBD: test for XOR multi-class-clasification