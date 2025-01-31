from neat_network import NeatNeuralNetwork
from population import Population
import matplotlib.pyplot as plt

X = [[0,0], [0,1], [1,0], [1,1]]      # each row is an example
Y = [[1, 0], [0, 1], [0, 1], [1, 0]]  # first out same, second out is diff [Same, Diff]

def main():
    pop_size = 50
    total_generations = 200
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

    print(f"{all_generations_fitness=}")
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
# - infinite loop in toplogy update, because of circular connection in add-connection-mutation
# - key error in self.Z[cur_node] = self.A[sour

"""
can you check my add connection method, remember input node cannot connect to another input node. An output node cannot connect to another output node. And a node cannot connect to a node creating a backwards cycle, just because there is a connection 4->5 doesnt mean you can connect 5->4. 

Add-Connection-Mutation
self.innovation_nums={1: '1->3', 6: '5->3', 7: '1->6', 8: '6->5', 4: '2->4', 2: '1->4', 9: '5->6'}
self.connections={3: ['1_IN1', '5_IN6'], 6: ['1_IN7', '5_IN9'], 5: ['6_IN8'], 4: ['2_IN4', '1_IN2']}
self.all_nodes=[1, 3, 5, 6, 2, 4]

Add-Node-Mutation
node we just added: 7 + n1.all_nodes=[1, 2, 4, 6, 5, 3, 7]
self.innovation_nums={3: '2->3', 6: '5->3', 2: '1->4', 7: '5->6', 8: '6->5', 4: '2->4'}
self.connections={3: ['2_IN3', '5_IN6'], 4: ['1_IN2', '2_IN4'], 6: ['5_IN7'], 5: ['6_IN8']}
self.all_nodes=[2, 3, 5, 1, 4, 6]
"""