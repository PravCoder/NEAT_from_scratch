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
- activation func in forward pass
- fitness func
- create pop-func called evolve_loop which is the evolutionary loop

BUG:
- link-to-split print is 0->0, cycle to itself input node when printing link-to-split in add node mutation func test
*/

/*
*****NOTES****

Even when randomly-connected for intailizing first generation of genomes, same links should have innovation numbers across genomes:
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

*/