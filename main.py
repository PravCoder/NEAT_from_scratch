from neat_network import NeatNeuralNetwork
from population import Population
import matplotlib.pyplot as plt

X = [[0,0], [0,1], [1,0], [1,1]]      # each row is an example
Y = [[1, 0], [0, 1], [0, 1], [1, 0]]  # first out same, second out is diff [Same, Diff]

def main():
    pop_size = 50
    total_generations = 100
    all_generations_fitness = []  # each element is average fitness of each generation

    input_nodes = [1,2]
    output_nodes = [3,4]
    IN = {1:"1->3", 2:"1->4", 3:"2->3", 4:"2->4"}
    

    p1 = Population(pop_size)
    p1.create_pop(IN=IN, input_nodes=input_nodes, output_nodes=output_nodes, bias_node_id=-1)
    for i in range(total_generations):
        p1.compute_pop_fitness(X, Y)
        all_generations_fitness.append(p1.average_fitness)
        p1.create_offsprings(tournament_size=3, elite_size=2, gen_num=i)

    plt.plot(all_generations_fitness, marker='o', linestyle='-', color='b', label='Average Fitness')
    plt.title('Average Fitness Over Generations')
    plt.xlabel('Generation')
    plt.ylabel('Average Fitness')
    plt.ylim(0, 1)  # Since fitness is between 0 and 1
    plt.grid(True)
    plt.legend()
    plt.show()


main()

# PROBLEMS:
# - infinite loop in toplogy update
# - key error in self.Z[cur_node] = self.A[sour