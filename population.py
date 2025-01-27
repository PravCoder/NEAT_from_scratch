import numpy as np
import random
from neat_network import NeatNeuralNetwork

class Population:
    def __init__(self, num_individuals):
        self.num_individuals = num_individuals
        self.genome_networks = []  # each element in neural-net-obj represents a indivudal genome in pop

    def create_pop(self): # creates networks for number of inviduals in population
        pass

    def select_best_genomes(self):
        pass

    def crossover_genomes(self, n1, n2):
        pass

    def find_matching_genes(n1, n2):
        pass

    def find_disjoint_genes(n1, n2, order="first"):
        pass

    def find_excess_genes(n1, n2, order="first"):
        pass

    def fitter_parent(n1, n2):
        if n1.fitness > n2.fitness:
            return n1.fitness
        elif n2.fitness > n1.ftiness:
            return n2.fitness
        else:
            return random.choice([n1, n2])
        

def main():
    p1 = Population(2)

    print("N1 FORWARD PROP")
    IN1 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4",8:"1->5"}
    n1 = NeatNeuralNetwork(innovation_nums=IN1, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal")
    n1.forward_propagation([0.1, 0.2, 0.3])  # out always 1 because of sigmoid


    print("N2 FORWARD PROP")
    IN2 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4D",6:"5->6",7:"6->4",9:"3->5",10:"1->6"}
    n2 = NeatNeuralNetwork(innovation_nums=IN2, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal")
    n2.forward_propagation([0.1, 0.2, 0.3])

main()