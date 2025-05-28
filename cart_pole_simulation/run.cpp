#include <iostream>
using namespace std;
#include "./Population.h" 
#include "CartPoleEnvironment.h" 
#include <sstream>

int main() {
    CartPoleEnvironment env;
    env.run_cartpole_neat_evolution();
}


/*
RUN: 
g++ -std=c++11 -o run run.cpp && ./run
*/