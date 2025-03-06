// First, let's define our basic structures

struct ConnectionGene {
    int inNode;
    int outNode;
    double weight;
    bool enabled;
    int innovation;  // Historical marking

    ConnectionGene(int in, int out, double w, bool en, int innov) 
        : inNode(in), outNode(out), weight(w), enabled(en), innovation(innov) {}
};

struct NodeGene {
    int id;
    std::string type;  // "input", "hidden", or "output"
    double activationValue;
    double bias;

    NodeGene(int nodeId, std::string nodeType) 
        : id(nodeId), type(nodeType), activationValue(0.0), bias(0.0) {}
};

// This represents a complete neural network
class Genome {
private:
    std::vector<NodeGene> nodes;
    std::vector<ConnectionGene> connections;
    int fitness;
    static int nextInnovation;

public:
    Genome(int numInputs, int numOutputs) {
        // Create input nodes
        for(int i = 0; i < numInputs; i++) {
            nodes.emplace_back(i, "input");
        }
        // Create output nodes
        for(int i = 0; i < numOutputs; i++) {
            nodes.emplace_back(numInputs + i, "output");
        }
        fitness = 0;
    }

    // Feed forward computation
    std::vector<double> feedForward(const std::vector<double>& inputs) {
        // Reset all activation values
        for(auto& node : nodes) {
            node.activationValue = 0.0;
        }

        // Set input values
        for(size_t i = 0; i < inputs.size(); i++) {
            nodes[i].activationValue = inputs[i];
        }

        // Process nodes in order (assuming feed-forward only)
        std::vector<double> outputs;
        for(const auto& node : nodes) {
            if(node.type != "input") {
                // Sum incoming connections
                double sum = node.bias;
                for(const auto& conn : connections) {
                    if(conn.enabled && conn.outNode == node.id) {
                        sum += nodes[conn.inNode].activationValue * conn.weight;
                    }
                }
                // Apply activation function (using sigmoid here)
                node.activationValue = 1.0 / (1.0 + exp(-sum));
            }
            if(node.type == "output") {
                outputs.push_back(node.activationValue);
            }
        }
        return outputs;
    }

    // Mutation: Add Node
    void addNodeMutation() {
        if(connections.empty()) return;
        
        // Choose a random connection
        int idx = rand() % connections.size();
        auto& conn = connections[idx];
        conn.enabled = false;

        // Create new node
        int newNodeId = nodes.size();
        nodes.emplace_back(newNodeId, "hidden");

        // Add two new connections
        connections.emplace_back(conn.inNode, newNodeId, 1.0, true, nextInnovation++);
        connections.emplace_back(newNodeId, conn.outNode, conn.weight, true, nextInnovation++);
    }

    // Mutation: Add Connection
    void addConnectionMutation() {
        // Find two nodes that aren't connected
        for(int attempts = 0; attempts < 100; attempts++) {
            int node1 = rand() % nodes.size();
            int node2 = rand() % nodes.size();
            
            // Ensure feed-forward (node1 must come before node2 in network)
            if(nodes[node1].type == "output" || nodes[node2].type == "input") continue;
            if(node1 == node2) continue;

            // Check if connection already exists
            bool exists = false;
            for(const auto& conn : connections) {
                if(conn.inNode == node1 && conn.outNode == node2) {
                    exists = true;
                    break;
                }
            }

            if(!exists) {
                double weight = (rand() % 2000 - 1000) / 1000.0;  // Random weight between -1 and 1
                connections.emplace_back(node1, node2, weight, true, nextInnovation++);
                break;
            }
        }
    }

    // Mutation: Mutate Weights
    void mutateWeights() {
        for(auto& conn : connections) {
            if(rand() % 100 < 90) {  // 90% chance of mutation
                conn.weight += (rand() % 200 - 100) / 1000.0;  // Small adjustment
                if(conn.weight > 1) conn.weight = 1;
                if(conn.weight < -1) conn.weight = -1;
            }
        }
    }

    // Crossover
    static Genome crossover(const Genome& parent1, const Genome& parent2) {
        // Assume parent1 is more fit
        Genome child(parent1.getNumInputs(), parent1.getNumOutputs());
        
        // Inherit all nodes
        child.nodes = parent1.nodes;

        // Inherit connections
        for(const auto& conn1 : parent1.connections) {
            bool found = false;
            for(const auto& conn2 : parent2.connections) {
                if(conn1.innovation == conn2.innovation) {
                    // Matching gene - randomly choose from either parent
                    if(rand() % 2 == 0) {
                        child.connections.push_back(conn1);
                    } else {
                        child.connections.push_back(conn2);
                    }
                    found = true;
                    break;
                }
            }
            if(!found) {
                // Disjoint or excess gene - inherit from more fit parent
                child.connections.push_back(conn1);
            }
        }
        return child;
    }

    int getNumInputs() const {
        int count = 0;
        for(const auto& node : nodes) {
            if(node.type == "input") count++;
        }
        return count;
    }

    int getNumOutputs() const {
        int count = 0;
        for(const auto& node : nodes) {
            if(node.type == "output") count++;
        }
        return count;
    }

    void setFitness(int f) { fitness = f; }
    int getFitness() const { return fitness; }
};

// Static member initialization
int Genome::nextInnovation = 0;

class Population {
    private:
        std::vector<Genome> genomes;
        int populationSize;
        double mutationRate;
        double crossoverRate;
        
        // Parameters for different mutation types
        struct MutationRates {
            double addNode = 0.03;
            double addConnection = 0.05;
            double weightMutation = 0.8;
        } mutationRates;
    
    public:
        Population(int size, int numInputs, int numOutputs) 
            : populationSize(size), mutationRate(0.3), crossoverRate(0.75) {
            // Initialize population with basic genomes
            for(int i = 0; i < size; i++) {
                genomes.emplace_back(numInputs, numOutputs);
                // Add some initial random connections
                for(int j = 0; j < numInputs * numOutputs; j++) {
                    genomes.back().addConnectionMutation();
                }
            }
        }
    
        // Evolve the population one generation
        void evolvePopulation() {
            // Sort genomes by fitness
            std::sort(genomes.begin(), genomes.end(), 
                [](const Genome& a, const Genome& b) {
                    return a.getFitness() > b.getFitness();
                });
    
            // Create new population starting with top performers (elitism)
            std::vector<Genome> newPopulation;
            int eliteCount = populationSize / 10;  // Keep top 10%
            for(int i = 0; i < eliteCount; i++) {
                newPopulation.push_back(genomes[i]);
            }
    
            // Fill rest of population with crossover and mutation
            while(newPopulation.size() < populationSize) {
                if(((double) rand() / RAND_MAX) < crossoverRate) {
                    // Perform crossover
                    Genome parent1 = selectParent();
                    Genome parent2 = selectParent();
                    Genome child = Genome::crossover(parent1, parent2);
                    mutateGenome(child);
                    newPopulation.push_back(child);
                } else {
                    // Clone and mutate
                    Genome child = selectParent();
                    mutateGenome(child);
                    newPopulation.push_back(child);
                }
            }
    
            genomes = newPopulation;
        }
    
        // Apply mutations to a single genome
        void mutateGenome(Genome& genome) {
            // Chance to add new node
            if(((double) rand() / RAND_MAX) < mutationRates.addNode) {
                genome.addNodeMutation();
            }
    
            // Chance to add new connection
            if(((double) rand() / RAND_MAX) < mutationRates.addConnection) {
                genome.addConnectionMutation();
            }
    
            // Chance to mutate weights
            if(((double) rand() / RAND_MAX) < mutationRates.weightMutation) {
                genome.mutateWeights();
            }
        }
    
        // Select parent for reproduction using tournament selection
        Genome selectParent() {
            int tournamentSize = 3;
            Genome best = genomes[rand() % genomes.size()];
            
            for(int i = 0; i < tournamentSize - 1; i++) {
                Genome competitor = genomes[rand() % genomes.size()];
                if(competitor.getFitness() > best.getFitness()) {
                    best = competitor;
                }
            }
            
            return best;
        }
    
        // Get best performing genome
        Genome& getBestGenome() {
            auto it = std::max_element(genomes.begin(), genomes.end(),
                [](const Genome& a, const Genome& b) {
                    return a.getFitness() < b.getFitness();
                });
            return *it;
        }
    
        // Evaluate the entire population using a fitness function
        void evaluatePopulation(std::function<double(Genome&)> fitnessFunc) {
            for(auto& genome : genomes) {
                double fitness = fitnessFunc(genome);
                genome.setFitness(fitness);
            }
        }
    
        // Get current generation's statistics
        struct PopulationStats {
            double averageFitness;
            double bestFitness;
            double worstFitness;
        };
    
        PopulationStats getStats() {
            PopulationStats stats;
            stats.bestFitness = genomes[0].getFitness();
            stats.worstFitness = genomes.back().getFitness();
            
            double sum = 0;
            for(const auto& genome : genomes) {
                sum += genome.getFitness();
            }
            stats.averageFitness = sum / genomes.size();
            
            return stats;
        }
    
        // Accessors
        int getPopulationSize() const { return populationSize; }
        const std::vector<Genome>& getGenomes() const { return genomes; }
    };