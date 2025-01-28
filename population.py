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

    def find_matching_genes(self, n1, n2): # order of genomes doesnt matter returns IN_nums common in both genomes
        matching_genes_IN_genes = []
        # iterate innovation_num genes of any genome if it exists in genes of other genome it is matching
        for in_num in n1.innovation_nums.keys():
            if in_num in list(n2.innovation_nums.keys()):
                matching_genes_IN_genes.append(in_num)
        return matching_genes_IN_genes

    def find_disjoint_genes(self, n1, n2):  # finds disjoint genes for first genome n1
        disjoint_genes_IN_nums = []
        # get the lowest & highest innovation numbers of other genome
        low_IN, high_IN = min(list(n2.innovation_nums.keys())), max(list(n2.innovation_nums.keys()))
        # iterate genes that exist in current genome
        for in_num in n1.innovation_nums.keys():
            # if cur-genome gene doesnst exist in other genome and is between low/highest innovation numbers of other genome it is disjoint
            if in_num not in list(n2.innovation_nums) and in_num > low_IN and in_num < high_IN:
                disjoint_genes_IN_nums.append(in_num)
        return disjoint_genes_IN_nums


    def find_excess_genes(self, n1, n2): # finds excess genes for first genome n1
        excess_genes_IN_nums = []
        # get highest innovation number of other genome
        high_IN = max(list(n2.innovation_nums.keys()))
        # iterate genes that exist in current genome
        for in_num in n1.innovation_nums.keys():
            # if cur-genome gene odesnt exist in other genome genes and is higher than highest innovation number of other genome it is excess
            if in_num not in list(n2.innovation_nums.keys()) and in_num > high_IN:
                excess_genes_IN_nums.append(in_num)
        return excess_genes_IN_nums

    def fitter_parent(self, n1, n2):
        if n1.fitness > n2.fitness:
            return n1.fitness
        elif n2.fitness > n1.ftiness:
            return n2.fitness
        else:
            return random.choice([n1, n2])
        

def main():
    p1 = Population(2)

    print("---N1 FORWARD PROP---")
    IN1 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4",8:"1->5"}
    n1 = NeatNeuralNetwork(innovation_nums=IN1, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal")


    print("---N2 FORWARD PROP---")
    IN2 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4D",6:"5->6",7:"6->4",9:"3->5",10:"1->6"}
    n2 = NeatNeuralNetwork(innovation_nums=IN2, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal")
    
    n1.forward_propagation([0.1, 0.2, 0.3])  # out always 1 because of sigmoid
    n2.forward_propagation([0.1, 0.2, 0.3])

    n1_n2_matching_genes = p1.find_matching_genes(n1, n2)
    print(f"{n1_n2_matching_genes=}")
    
    n1_disjoint_genes = p1.find_disjoint_genes(n1, n2)
    n1_excess_genes = p1.find_excess_genes(n1, n2)
    print(f"{n1_disjoint_genes=}")
    print(f"{n1_excess_genes=}")

    n2_disjoint_genes = p1.find_disjoint_genes(n2, n1)
    n2_excess_genes = p1.find_excess_genes(n2, n1)
    print(f"{n2_disjoint_genes=}")
    print(f"{n2_excess_genes=}")

main()