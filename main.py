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
LOG OF ERRORS:

1.
Circular connection causing infinite loop when creating offspring object.
offspring_IN={1: '1->3D', 2: '1->4', 3: '2->3', 4: '2->4', 5: '1->5', 6: '5->4', 7: '4->5'}
IS THIS LAST THING THATS PRINTED

2. 
-----ADD NODE MUTATION-----
--before
n1.innovation_nums={1: '1->3D', 2: '1->4', 3: '2->3', 4: '2->4D', 5: '1->3', 6: '5->3'}
n1.weights={1: np.float64(-0.5198719984532977), 2: np.float64(0.7583981677823782), 3: np.float64(-2.775146536892831), 4: np.float64(-1.7720150046806062), 5: 1, 6: 0.15185324061515662, 7: 0.10596686024509488, 8: 0.013710999243000274, 9: 1, 10: 0.035057312104959104}
n1.connections={4: ['1_IN2'], 3: ['2_IN3', '1_IN5', '5_IN6']}
n1.all_nodes=[1, 2, 4, 5, 3]
did not add node mutation
Traceback (most recent call last):
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/main.py", line 36, in <module>
    main()
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/main.py", line 21, in main
    p1.compute_pop_fitness(X, Y)
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/population.py", line 22, in compute_pop_fitness
    cur_genome.fitness_evaluation_XOR(X, Y)   # compute fitness of each genome, WARNING: fitness might be same for all genomes
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/neat_network.py", line 218, in fitness_evaluation_XOR
    self.forward_propagation(X_example)  # updates self.A activations with cur example input activations
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/neat_network.py", line 198, in forward_propagation
    output_Z_vector.append(self.Z[cur_node])  # add cur-output-node-z-value to vector in order
KeyError: 3

3. 
-----ADD NODE MUTATION-----
--before
n1.innovation_nums={1: '1->3', 2: '1->4D', 3: '2->3', 4: '2->4', 5: '2->5', 6: '5->3', 7: '1->6'}
n1.weights={1: np.float64(1.589542250006221), 2: np.float64(-1.847807272302246), 3: np.float64(0.8944675629739487), 4: np.float64(0.08698744706140611), 5: 1.2335814545951902, 6: -0.41381415704489244, 7: -0.18829470954200406, 8: 1, 9: -0.10461732207031534, 10: 0.16613124289520761}
n1.connections={3: ['1_IN1', '2_IN3', '5_IN6'], 4: ['2_IN4'], 5: ['2_IN5'], 6: ['1_IN7']}
n1.all_nodes=[1, 2, 4, 5, 6, 3]
FLAG1
^CTraceback (most recent call last):
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/main.py", line 36, in <module>
    main()
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/main.py", line 23, in main
    p1.create_offsprings(tournament_size=3, elite_size=2, gen_num=i)
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/population.py", line 81, in create_offsprings
    self.add_node_mutation(offspring)
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/population.py", line 318, in add_node_mutation
    n1.prepare_network() # updates toplogical order of nodes with new-node, and sef.connections of each node, the sources of each target. 
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/neat_network.py", line 79, in prepare_network
    self.all_nodes = self.get_topological_order()
  File "/Users/pravachanpatra/Documents/PYTHON/AI_ML_DL/NEAT_algorithm_scratch/neat_network.py", line 132, in get_topological_order
    if all(dep in processed for dep in dependencies[node]):
KeyboardInterrupt
Infinite loop 2 -> 5 -> 3 and 2 -> 3. 


NOTE:
if two parents have the same innovation number, they must have the same connection
"""