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


02/06/2025: This implementation does not work, this is a failed attempt. 

