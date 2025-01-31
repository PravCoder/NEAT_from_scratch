import numpy as np
import random
import re
from neat_network import NeatNeuralNetwork

class Population:
    def __init__(self, num_individuals):
        self.num_individuals = num_individuals
        self.genome_networks = []  # each element in neural-net-obj represents a indivudal genome in pop
        self.average_fitness = 0
        self.best_fitness = 0

    def create_pop(self, IN, input_nodes, output_nodes, bias_node_id): # creates networks for number of inviduals in population
        for _ in range(self.num_individuals):
            n1 = NeatNeuralNetwork(innovation_nums=IN, input_nodes=input_nodes,output_nodes=output_nodes, bias_node_id=bias_node_id, seed_individual=True, initializer="glorot_normal") # first generation genome randomly initalize weights
            self.genome_networks.append(n1)
    
    def compute_pop_fitness(self, X, Y):
        # compute fitness of each genome
        for i, cur_genome in enumerate(self.genome_networks):
            #print(f"\nEvaluating Fitness of {i}th Genome:") # AOGFB
            cur_genome.fitness_evaluation_XOR(X, Y)   # compute fitness of each genome, WARNING: fitness might be same for all genomes
            #print(f"fitness: {cur_genome.fitness}")  # AOGFB
            self.average_fitness += cur_genome.fitness
            self.best_fitness = max(self.best_fitness, cur_genome.fitness)
        self.average_fitness /= self.num_individuals
        #print(f"average fitness: {self.average_fitness}") # AOGFB
        #print(f"best fitness: {self.best_fitness}") # AOGFB

    def select_best_genomes(self, tournament_size, elite_size):
        # create population with fitness tuples
        selected_parents = []
        population = [(genome, genome.fitness) for genome in self.genome_networks]
        # sort population by fitness in descending order
        sorted_population = sorted(population, key=lambda x: x[1], reverse=True)
        # elitism - directly add top performers
        elites = [genome for genome, _ in sorted_population[:elite_size]]
        selected_parents.extend(elites)
        # tournament selection for remaining spots
        num_selections_needed = self.num_individuals - elite_size
        
        for _ in range(num_selections_needed):
            tournament_candidates = random.sample(sorted_population, tournament_size)
            winner = max(tournament_candidates, key=lambda x: x[1])[0]  # Get genome with highest fitness
            selected_parents.append(winner)

        return selected_parents
    
    def create_offsprings(self, tournament_size, elite_size, gen_num):
        print(f"\nGENERATION {gen_num}th STATS:")
        print(f"Average fitness: {self.average_fitness}")
        print(f"Best fitness: {self.best_fitness}\n")

        parents = self.select_best_genomes(tournament_size=tournament_size, elite_size=elite_size)
        # create new population starting with elites
        offsprings = parents[:elite_size]  # Keep elites unchanged
        
        # generate remaining offspring through crossover
        num_offspring_needed = self.num_individuals - elite_size
        
        # pair parents for crossover
        for _ in range(num_offspring_needed):
            # randomly select two different parents
            parent1, parent2 = random.choice(parents), random.choice(parents)
            #print("----------CROSSOVER PARENTS----------") # AOGFB
            
            # create offspring through crossover
            offspring = self.crossover_genomes(parent1, parent2)
            #print("-Offpring Original Info:")      # AOGFB
            #print(f"{offspring.innovation_nums=}") # AOGFB
            #print(f"{offspring.weights=}") # AOGFB
            
            #print("\n-Offspring Mutation Info") # AOGFB
            rand_num = random.random()
            if rand_num < 0.30: # 80% chance ->
                self.weight_mutation(offspring)
            elif rand_num < 0.60: # 5% chance -> add connection
                self.add_connection_mutation(offspring)
            elif rand_num < 1.00:  # 3% chance ->  add node mutation
                self.add_connection_mutation(offspring)
            
            offsprings.append(offspring)
            #print(f"{offspring.innovation_nums=}") # AOGFB
            #print(f"{offspring.weights=}") # AOGFB

        # set offsprigns as new population
        self.genome_networks = offsprings
        self.num_individuals = len(self.genome_networks)
        self.average_fitness = 0
        self.best_fitness = 0


    def crossover_genomes(self, n1, n2):
        offspring_IN = {}  # {innovation-num: connection-string}
        offspring_weights = {}  # {innnovation_num: weight-value}
        n1_n2_matching_genes = self.find_matching_genes(n1, n2)

        n1_disjoint_genes = self.find_disjoint_genes(n1, n2)
        n1_excess_genes = self.find_excess_genes(n1, n2)
   
        n2_disjoint_genes = self.find_disjoint_genes(n2, n1)
        n2_excess_genes = self.find_excess_genes(n2, n1)

        # inherit each individual matching gene randomly inherit from either parent
        for in_num in n1_n2_matching_genes:  # for each matching-gene-innovation-num randomly choose one of the parent genomes 
            rand_genome = random.choice([n1, n2]) 
            offspring_IN[in_num] = rand_genome.innovation_nums[in_num] # save randomly chosen genomes innovation-num:"source->target"
            offspring_weights[in_num] = rand_genome.weights[in_num]   # save randomly chosen genomes innovation-num:weight-value

        # inherit all disjoint/excess genes from fitter parent
        if n1.fitness > n2.fitness:
            for in_num in n1_disjoint_genes: # iterate all n1 disjoint-gnees
                offspring_IN[in_num] = n1.innovation_nums[in_num]  # {innovation-num: "source->target"}
                offspring_weights[in_num] = n1.weights[in_num]     # {innovaiton-num: weight-value}
            for in_num in n1_excess_genes:  # iterate all n1 exces-genes
                offspring_IN[in_num] = n1.innovation_nums[in_num]
                offspring_weights[in_num] = n1.weights[in_num] 
        elif n2.fitness > n1.fitness:
            for in_num in n2_disjoint_genes:  # iterate all n2 disjoint-genes
                offspring_IN[in_num] = n2.innovation_nums[in_num]
                offspring_weights[in_num] = n2.weights[in_num] 
            for in_num in n2_excess_genes:    # iterate all n2 excess-genes
                offspring_IN[in_num] = n2.innovation_nums[in_num]
                offspring_weights[in_num] = n2.weights[in_num] 
        else:  # when fitness is equal random decision is made to inherit all excess/disjoint from either parent
            #print("yello")
            rand_genome = random.choice([n1, n2]) 
            if rand_genome == n1:
                #print("bello")
                all_disjoint_excess_genes_from_a_genome = n1_disjoint_genes+n1_excess_genes
                for in_num in all_disjoint_excess_genes_from_a_genome:
                    offspring_IN[in_num] = n1.innovation_nums[in_num] # save randomly chosen genomes innovation-num:"source->target"
                    offspring_weights[in_num] = n1.weights[in_num]   # save randomly chosen genomes innovation-num:weight-value
            elif rand_genome == n2:
                #print("dello")
                all_disjoint_excess_genes_from_a_genome = n2_disjoint_genes+n2_excess_genes
                for in_num in all_disjoint_excess_genes_from_a_genome:
                    offspring_IN[in_num] = n2.innovation_nums[in_num] # save randomly chosen genomes innovation-num:"source->target"
                    offspring_weights[in_num] = n2.weights[in_num]   # save randomly chosen genomes innovation-num:weight-value
            
        offspring = NeatNeuralNetwork(innovation_nums=offspring_IN, input_nodes=n1.input_nodes,output_nodes=n1.output_nodes, bias_node_id=n1.bias_node_id, 
                               seed_individual=False)
        # print(f"{offspring.innovation_nums=}")
        # print(f"{offspring.weights=}")
        return offspring

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
        # iterate genes that exist in current genome, optimization: Instead of iterating over all keys in n1.innovation_nums.keys(), you could use a filter step that excludes keys below high_IN
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
        
    def extract_node_and_innovation(self, s):
        match = re.match(r"(\d+)_IN(\d+)", s)
        if match:
            node_id = int(match.group(1))  
            innovation_num = int(match.group(2)) 
            return node_id, innovation_num
        return None
        
    def weight_mutation(self, n1):
        #print("Weight-Mutation") # AOGFB
        # iterate all innovation-numbers of genome and add small random guassian distribution value to its weight value
        for in_num in n1.innovation_nums.keys():
            n1.weights[in_num] += random.gauss(0, 0.1)
        return n1
    
    def add_connection_mutation(self, n1):
        #print("Add-Connection-Mutation") # AOGFB
        existing_connections = {}  # {target-node-id: [source1-id, source2-id], node1: [s1,s2,s3]|
        nonexistent_connections = []  # list of tuples [ (source,target), (s,t)]
        
        # populate existing connections dict
        for target_node_id, connection_arr in n1.connections.items():
            existing_connections[target_node_id] = []
            for connection_str in connection_arr:
                source_node_id, _ = self.extract_node_and_innovation(connection_str)
                existing_connections[target_node_id].append(source_node_id)
        
        top_sorted_nodes = n1.all_nodes  # [1, 2, 3, 5, 6, 4]
        
        # for each pair of nodes in topological order
        for i, source_node in enumerate(top_sorted_nodes):
            # if source node is not an input node 
            if source_node not in n1.input_nodes:
                # only consider target nodes that come after source in topological order, every other node
                for target_node in top_sorted_nodes[i+1:]:
                    # skip if target node is an input node or if both source and target are output nodes
                    if target_node not in n1.input_nodes and not (source_node in n1.output_nodes and target_node in n1.output_nodes):
                        # check if this connection already exists
                        if target_node in existing_connections:
                            if source_node not in existing_connections[target_node]:
                                # connection doesn't exist and respects topological order
                                nonexistent_connections.append((source_node, target_node))
                        else:
                            # target node has no incoming connections at all
                            nonexistent_connections.append((source_node, target_node))
            else:  # if source node is an input node
                # only consider non-input nodes as targets, because can't have (input, input)
                for target_node in top_sorted_nodes[i+1:]:
                    if target_node not in n1.input_nodes:
                        if target_node in existing_connections:
                            if source_node not in existing_connections[target_node]:
                                nonexistent_connections.append((source_node, target_node))
                        else:
                            nonexistent_connections.append((source_node, target_node))

        # If there are valid connections, select one randomly
        if nonexistent_connections:
            rand_connection = random.choice(nonexistent_connections)  # choose a random non-existent connection
            #print(f"Random Connection: {rand_connection}")  # AOGFB
            rand_source, rand_target = rand_connection[0], rand_connection[1]
            new_innovation_num = n1.update_max_IN() + 1

            n1.innovation_nums[new_innovation_num] = f"{rand_source}->{rand_target}"  # {in-num: "s->target"}
            
            n1.weights[new_innovation_num] = random.gauss(0, 0.1)

            # add connection to connections-dict
            if new_innovation_num not in n1.connections.keys():
                n1.connections[new_innovation_num] = [f"{rand_source}_IN{new_innovation_num}"]
            else:
                n1.connections[new_innovation_num].append(f"{rand_source}_IN{new_innovation_num}")
        
        return n1

    
    def add_node_mutation(self, n1):
        #print("Add-Node-Mutation") # AOGFB
        # after adding node, topologically sort all nodes again
        # run prepare-network again to comptue all of its connections
        
        random_IN_item = random.choice(list(n1.innovation_nums.items()))  # chose random connection-item in innovation-num
        random_innovation_num = random_IN_item[0]
        rand_source, rand_target = int(random_IN_item[1].split('->')[0]), int(random_IN_item[1].split('->')[1]) # access 1 for value to get s->t, TBD: make sure random chosen connection doesnt have D
        #print(f"**Random connection to split: {random_IN_item=}, {rand_source=}, {rand_target=}") # AOGFB
        
        # disable random connection via innovation-num
        # n1.innovation_nums[random_innovation_num] += "D"
        del n1.innovation_nums[random_innovation_num]   # temporarily delete because teh above split not working for D in string
        
        # create new node and add it to all nodes
        new_node_id = n1.get_max_node_id() + 1
        n1.all_nodes.append(new_node_id)

        # create connection from original source node to new node
        new_innovation_num1 = n1.update_max_IN() + 1
        n1.innovation_nums[new_innovation_num1] = f"{rand_source}->{new_node_id}"
        n1.weights[new_innovation_num1] = 1  # set original->new node weight to 1

        # create connection from new-node to original target node
        new_innovation_num2 = n1.update_max_IN() + 1
        n1.innovation_nums[new_innovation_num2] = f"{new_node_id}->{rand_target}"
        n1.weights[new_innovation_num2] = random.gauss(0, 0.1)  # set new-node->target-og weight to small value

        n1.prepare_network() # updates toplogical order of nodes with new-node, and sef.connections of each node, the sources of each target. 
        #print(f"**{new_innovation_num1=}, {new_innovation_num2=}") # AOGFB
        return n1

def main():
    test_crossover=False
    test_weight_mutation=False
    test_add_connection_mutation=True
    test_add_node_mutation=False

    p1 = Population(2)

    # example of 2 parent networks in paper. 
    print("--------N1 FORWARD PRO/INIT--------")
    IN1 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4",8:"1->5"}
    n1 = NeatNeuralNetwork(innovation_nums=IN1, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal", fitness=1)
    n1.forward_propagation([0.1, 0.2, 0.3])  # out always 1 because of sigmoid

    print("--------N2 FORWARD PROP/INIT--------")
    IN2 = {1:"1->4",2:"2->4D",3:"3->4",4:"2->5",5:"5->4D",6:"5->6",7:"6->4",9:"3->5",10:"1->6"}
    n2 = NeatNeuralNetwork(innovation_nums=IN2, input_nodes=[1,2,3],
                           output_nodes=[4], bias_node_id=-1, 
                           seed_individual=True, initializer="glorot_normal", fitness=2)
    n2.forward_propagation([0.1, 0.2, 0.3])

    if test_crossover:
        print("\n***** Test Crossover *****")
        n1_n2_matching_genes = p1.find_matching_genes(n1, n2)
        print(f"{n1_n2_matching_genes=}")

        n1_disjoint_genes = p1.find_disjoint_genes(n1, n2)
        n1_excess_genes = p1.find_excess_genes(n1, n2)
        print(f"{n1_disjoint_genes=}")
        print(f"{n1_excess_genes=}")

        n2_disjoint_genes = p1.find_disjoint_genes(n2, n1)
        n2_excess_genes = p1.find_excess_genes(n2, n1)
        print(f"{n2_disjoint_genes=}")
        print(f"{n2_excess_genes=}\n")

        print("Crossover results below:")
        p1.crossover_genomes(n1, n2)
    
    if test_weight_mutation:
        print("\n***** Test Weight Mutation *****")
        print(f"Before weights: {n1.weights}")
        p1.weight_mutation(n1)
        print(f"\nAfter weights: {n1.weights}")

    if test_add_connection_mutation:
        print("\n***** Test Add Connection Mutation *****")
        print("N1")
        print(f"Before: {n1.innovation_nums=}")
        p1.add_connection_mutation(n1)
        print(f"After: {n1.innovation_nums=}")

        print("\nN2")
        print(f"Before: {n2.innovation_nums=}")
        p1.add_connection_mutation(n2)
        print(f"After: {n2.innovation_nums=}")

    if test_add_node_mutation:
        print("\n***** Test Add Node Mutation *****")
        print("N1")
        print(f"Before: {n1.innovation_nums=}")
        p1.add_node_mutation(n1)
        print(f"After: {n1.innovation_nums=}")
        print(f"New Connections: {n1.connections}") # make sure new node connections are in sources
        print(f"New Weights: {n1.weights}") # make sure new connection weights are there

        print("\nN2")
        print(f"Before: {n2.innovation_nums=}")
        p1.add_node_mutation(n2)
        print(f"After: {n2.innovation_nums=}")
        print(f"New Connections: {n2.connections}") # make sure new node connections are in sources
        print(f"New Weights: {n2.weights}") # make sure new connection weights are there

#main()