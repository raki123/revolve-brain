#ifndef NEAT_CONNECTION_GENE_H_
#define NEAT_CONNECTION_GENE_H_

#include "gene.h"
//class combining meta and normal informations for neurons
namespace CPPNEAT {
class ConnectionGene : public Gene {

public:
	ConnectionGene(int mark_to, int mark_from, double weight, int innov_number = 0, bool enabled = true, std::string socket = "") 
		: Gene(innov_number, enabled)
		, mark_to(mark_to)
		, mark_from(mark_from)
		, weight(weight)
		, socket(socket) { this->gene_type = Gene::CONNECTION_GENE; };
	ConnectionGene(ConnectionGene &copy_of) 
		: Gene(copy_of.getInnovNumber(), copy_of.isEnabled())
		, mark_to(copy_of.mark_to)
		, mark_from(copy_of.mark_from)
		, weight(copy_of.weight)
		, socket(copy_of.socket) { this->gene_type = Gene::CONNECTION_GENE; }
public:
	int mark_to;
	int mark_from;
	double weight;
	std::string socket;
}; 
}

#endif //NEAT_CONNECTION_GENE_H_