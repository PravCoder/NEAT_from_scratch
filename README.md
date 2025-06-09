# NEAT Algorithm from Scratch

This is a implementation of the NEAT paper: https://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf

NEAT stands for Neuroevolution of augmenting topologies, it is a method for evolving the archtiecture/toplogy of neural network through a evolutionary algorithm to solve some problem.

NEAT Algorithm Steps:
1. Start with initial population of genomes simple neural networks. 
2. Compute fitness of each genome in population. 
3. Select the best performing network-genomes of population. 
4. Pair the best networks & create offspring by taking genes from one parent & some genes from another parent.
5. Randomly mutate some of the offspring genomes at random to add variation. Mutation is post crossover offsprings.
6. Place all of these offsprings in a new population. 
7. Calculate fitness of the individuals of the new population.
8. Repeat from step 3 using the new population.

Types of Mutations Implemented:
1. Add Connection Mutation: where a random connection between 2 unconnected nodes is created.
2. Add Node Mutation: where a random connection is selected and split with a new node, creating 2 new connections from the  source to the new node and from the new node to the target.
3. Weight Mutation.

Crossover of Neural Networks Process:
1. 

Code Structure: Population.h, Genome.h, NodeGene.h, LinkGene.h.


How to run:
Go to tests.cpp in root folder and run this command: g++ -std=c++11 -o tests tests.cpp && ./tests
This will run the neat evolution aglorithm on the XOR problem and print the results at the end, of the best genome.

02/06/2025: This implementation now works, this is a sucessful attempt. 

