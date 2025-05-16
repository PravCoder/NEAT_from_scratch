#include <iostream>
using namespace std;
#include "Population.h" 

int main() {

}

/*
g++ -o main main.cpp
./main
g++ -o main main.cpp && ./main
g++ -std=c++11 -o main main.cpp && ./main
*/

/*
TODO:
- evolve_loop, do elistiem preserving the best individuals unchanged
- crossover test
- different activation func in forward pass
- get_best_solution() function which gets best performing genome from last generation

THIS IS A LOG OF ALL THE SIGNIFICANT-ROADBLOCK-BUGS IVE ENCOUNTERED DURING THIS PROJECT, DOCUMENTING THEM AND WRITING DOWN MY THOUGHTS:
[-] population is halving every generation, decide how many are selected for repdocution, when these are paired decide how many offsprings each pairing generates. 
-  segmentation error, because empty network is being passed in mutation-add-node. Theres two ways a empty-genome can occur one with 
   I wasn't updatin thte population_size variable when a new population is created so when I select the best networks I as selecting a invalid index
   crossover and other because of memory management error. But for the crossover its parent that it receives is always empty, 
    suggesting that this empty network emrged from another mutation like add node.
    Check: checked if mutations are transforming a non empty offspring into a empty offspring, which they seem to be not, check the implementation of this
    ***Empty Network Is Being Produced By Crossver, even when its parents are not empty. Bug in crossover print edge case.
    Latest: empty-networks are not being created in crossover, but are being given to mutations. Currently mutations handle empty networks if they are given thats why number of empty is not printed per generation.
      The intermediaty functions in the evoltion loop are causing empty networks, selected genomes has empty networks
      Check Activation functions for this task
    Temporary Fix: if given node in mutation functions is empty randomly initialize it.
-  activation functions for this type of network. Try only one output node.
- best_fitness is 1: the networks found a loophole with no hidden nodes or links which tehcnically creates a 0 error which results in fitness of 1
   somehow empty networks are being born after some generations even if first gen doesnt have empty networks
      maybe due to fitness function rewarding empty networks, incorrect crossover implementation.
      even the input/output nodes arent being printed.
      its because the first empty network is because cloning is not done properly
      maybe a mutation is accidently removing connections check by printing given genome and created genome only if given is not empty and created is empty
      maybe crossover but the given parent is always empty so offspring is reuslting empty but where is this parent empty network emerging from
   Fixed: initial population has same genomes with same links - Fixed with random seed at program entry
   Fixed: crossing over 2 parents is always resulting in second parent, because fitness of both parents is 0  - Fixed by if fitness is saem randomlly choose fitter parent
- weights are exploding lol like in a standard network :( 
  Fixed: capped weights during mutation, because mutation was adding to the weights which results in significant accumulation over nukmerous generations. 
- max_attempts in add connection is negative, max_attempts: -10 sometimes.
- sometimes there is segmentation fault after this is printed: average_fitness: 0.2
- link-to-split print is 0->0, cycle to itself input node when printing link-to-split in add node mutation func test







----------NOTES----------:

INITIAL POPULATION: Even when randomly-connected for intailizing first generation of genomes, same links should have innovation numbers across genomes:
Population size: 5
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->2[IN=0]
   1->2[IN=1]
   1->3[IN=2]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->3[IN=2]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->3[IN=3]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->2[IN=0]
   0->3[IN=3]
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->3[IN=2]







CROSSOVER EXAMPLE:
---Crossover Two Parent Genomes Test:---
Parent 1: 
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   0->3[IN=0]e
   1->2[IN=1]e
   1->3[IN=2]e
Parent 2: 
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->2[IN=1]e
   1->3[IN=2]e
Offspring: 
genome in: 2 outs: 2
   nodes: 0-inp, 1-inp, 2-out, 3-out
   links: 
   1->2[IN=1]e
   1->3[IN=2]e

*/