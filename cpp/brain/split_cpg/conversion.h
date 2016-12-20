#ifndef REVOLVE_BRAIN_CONVERSION_H_
#define REVOLVE_BRAIN_CONVERSION_H_

#include "../cppneat/genetic_encoding.h"
#include "../split_cpg/ext_nn.h"
#include "../cppneat/neat_learner.h"

namespace revolve {
namespace brain {

//used for communication between cppneat learner and ext nn controller
extern CPPNEAT::Learner::LearningConfiguration learning_configuration;
extern std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

extern std::map<int, unsigned int> input_map;
extern std::map<int, unsigned int> output_map;

void set_brain_spec();
void set_learning_conf();
boost::shared_ptr<ExtNNConfig> convertForController(CPPNEAT::GeneticEncodingPtr genotype);
CPPNEAT::GeneticEncodingPtr convertForLearner(boost::shared_ptr<ExtNNConfig> config);
}
}

#endif /* REVOLVE_BRAIN_CONVERSION_H_ */
