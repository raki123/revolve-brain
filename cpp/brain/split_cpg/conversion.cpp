#include "conversion.h"
#include <map>

namespace revolve {
namespace brain {

std::map<ExtNNController1::ExtNNConfig, CPPNEAT::GeneticEncodingPtr> known;

ExtNNController1::ExtNNConfig convertForController(CPPNEAT::GeneticEncodingPtr genotype) {
	std::vector<NeuronGenePtr> neuron_genes = genotype->neuron_genes;
	std::vector<ConnectionGenePtr> connection_genes = genotype->connection_genes;
	
}
CPPNEAT::GeneticEncodingPtr convertForLearner(ExtNNController1::ExtNNConfig config) {
	return known[config];
}
}	
}