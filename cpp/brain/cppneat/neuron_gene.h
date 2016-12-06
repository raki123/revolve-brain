#ifndef NEAT_NEURON_GENE_H_
#define NEAT_NEURON_GENE_H_

#include "gene.h"
#include "neuron.h"
#include "types.h"

//class combining meta and normal informations for neurons
namespace CPPNEAT {
class NeuronGene : public Gene {

public:
	NeuronGene(NeuronPtr neuron, int innov_number = 0, bool enabled = true) 
		: Gene(innov_number, enabled)
		, neuron(neuron) { this->gene_type = Gene::NEURON_GENE; }
	NeuronGene(NeuronGene &copy_of) 
		: Gene(copy_of.getInnovNumber(), copy_of.isEnabled())
		, neuron(NeuronPtr(new Neuron(*(copy_of.neuron)))) { this->gene_type = Gene::NEURON_GENE; }
public:
	NeuronPtr neuron;
}; 
}

#endif //NEAT_NEURON_GENE_H_