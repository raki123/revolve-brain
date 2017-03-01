#ifndef NEAT_NEURON_GENE_H_
#define NEAT_NEURON_GENE_H_

#include "Genome.h"
#include "Neuron.h"
#include "Types.h"

//class combining meta and normal informations for neurons
namespace CPPNEAT {
class NeuronGene : public Gene {

public:
	NeuronGene(NeuronPtr neuron, int innov_number = 0, bool enabled = true, std::string parent_name = "", int parent_index = -1) 
		: Gene(innov_number, enabled, parent_name, parent_index)
		, neuron(neuron) { this->gene_type = Gene::NEURON_GENE; }
	NeuronGene(NeuronGene &copy_of) 
		: Gene(copy_of.getInnovNumber(), copy_of.isEnabled(), copy_of.get_parent_name(), copy_of.get_parent_index())
		, neuron(NeuronPtr(new Neuron(*(copy_of.neuron)))) { this->gene_type = Gene::NEURON_GENE; }
public:
	NeuronPtr neuron;
}; 
}

#endif //NEAT_NEURON_GENE_H_