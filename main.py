from neat_network import NeatNeuralNetwork
from population import Population

X = [[0,0], [0,1], [1,0], [1,1]]      # each row is an example
Y = [[1, 0], [0, 1], [0, 1], [1, 0]]  # first out same, second out is diff [Same, Diff]

def main():
    pop_size = 5
    total_generations = 10

    input_nodes = [1,2]
    output_nodes = [3,4]
    IN = {1:"1->3", 2:"1->4", 3:"2->3", 4:"2->4"}

    print("-----CREATE FIRST POPULATION-----")
    p1 = Population(pop_size)
    p1.create_pop(IN=IN, input_nodes=input_nodes, output_nodes=output_nodes, bias_node_id=-1)
    p1.compute_pop_fitness(X, Y)


main()