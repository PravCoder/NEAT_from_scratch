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

def main():
    input_nodes = 2
    output_nodes = 2

    IN = {1:"1->3", 2:"1->4", 3:"2->4", 4:"5->6" , 5:"2->7"}
    bias_node = {6: "8->5"}  # bias node can have multiple connections to different nodes
    
    n1 = NeatNeuralNetwork(innovation_nums=IN, input_nodes=input_nodes,output_nodes=output_nodes, bias_node=bias_node, seed_individual=True, initializer="glorot_normal")
    n1.initialize_weights()

main()

# [?] how is topology of network initalized