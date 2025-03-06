
#ifndef Genome_H
#define Genome_H

/*
Represents a individual in population which is a network with nodes and links/
*/
class Genome {

    public:
        vector<NodeGene> nodes:
        vector<LinkGene> links;
        static int next_innovation_num;

}


#endif 