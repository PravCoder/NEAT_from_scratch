import numpy as np

class NeatNeuralNetwork:

    def __init__(self, innovation_nums, input_nodes, output_nodes, weights={}, bias_node={}, seed_individual=False, initializer=None):
        self.innovation_nums = innovation_nums
        self.input_nodes = input_nodes
        self.output_nodes = output_nodes
        self.weights = weights
        self.bias_node = bias_node
        self.seed_individual = seed_individual
        self.initializer = initializer
        if self.seed_individual == True: self.initialize_weights()
        self.connections = {} # {node-id: [connected from nodes]}, {target: [source-ids]}, {'3': ['1_IN1'], '4': ['1_IN2', '2_IN3'], '6': ['5_IN4'], '7': ['2_IN5']}, has the from-source node and innovation number of that connection in the value of each target node key
        self.max_IN = self.update_max_IN()

    def initialize_weights(self):
        num_weights = len(self.innovation_nums.keys())
        if self.initializer == "normal":
            distribution = np.random.randn(num_weights)
        if self.initializer == "glorot_normal":
            std = np.sqrt(2 / (self.input_nodes + self.output_nodes))
            distribution = np.random.randn(num_weights) * std
        if self.initializer == "glorot_uniform":
            limit = np.sqrt(6 / (self.input_nodes + self.output_nodes))  # Glorot uniform initialization
            distribution = np.random.uniform(-limit, limit, num_weights)
        if self.initializer == "he_normal":
            std = np.sqrt(2 / self.input_nodes)  # He normal initialization
            distribution = np.random.randn(num_weights) * std
        if self.initializer == "he_uniform":
            limit = np.sqrt(6 / self.input_nodes)  # He uniform initialization
            distribution = np.random.uniform(-limit, limit, num_weights)

        for i, in_num in enumerate(list(self.innovation_nums.keys())):
            self.weights[in_num] = distribution[i]

        print(f"Initalized weights: {self.weights}")
        return self.weights
    
    """
    For each node in network creates connections dict, where each key is node-id and its value is list of the from-source nodes connected to it
    and that corresponding target node source node IN number so we can look up the weight value efficently
    """
    def prepare_network(self):
        for in_num, connection_str in list(self.innovation_nums.items()):

            source, target =  connection_str.split("->")[0], connection_str.split("->")[1]

            source_str = f"{source}_IN{in_num}"
            if target not in list(self.connections):
                self.connections[target] = [source_str]
            elif target in list(self.connections):
                self.connections[target].append(source_str)

        print(f"Connections: {self.connections}")
        return self.connections
    
    def update_max_IN(self):
        self.max_IN = max(list(self.innovation_nums.keys()))
        return self.max_IN

def main():
    input_nodes = 2
    output_nodes = 2

    IN = {1:"1->3", 2:"1->4", 3:"2->4", 4:"5->6" , 5:"2->7"}  # each connection-str it is source-node-id->target-node-id
    bias_node = {6: "8->5"}  # bias node can have multiple connections to different nodes
    
    n1 = NeatNeuralNetwork(innovation_nums=IN, input_nodes=input_nodes,output_nodes=output_nodes, bias_node=bias_node, seed_individual=True, initializer="glorot_normal")
    n1.prepare_network()

main()

# [?] how is topology of network initalized