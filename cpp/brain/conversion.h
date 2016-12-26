#ifndef REVOLVE_BRAIN_CONVERSION_H_
#define REVOLVE_BRAIN_CONVERSION_H_

#include "learner/cppneat/genetic_encoding.h"
#include "controller/ext_nn_net.h"
#include "controller/layered_ext_nn_net.h"
#include "learner/neat_learner.h"

namespace revolve {
namespace brain {

//used for communication between cppneat learner and ext nn net controller
//the conversion methods work only when using standard neat
extern CPPNEAT::Learner::LearningConfiguration learning_configuration;
extern std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

extern std::map<int, unsigned int> input_map;
extern std::map<int, unsigned int> output_map;

void set_brain_spec(bool hyperneat);
void set_learning_conf();
boost::shared_ptr<ExtNNConfig> convertForController(CPPNEAT::GeneticEncodingPtr genotype);
CPPNEAT::GeneticEncodingPtr convertForLearner(boost::shared_ptr<ExtNNConfig> config);



//used for communication between rlpower learner and ext nn weights controller
std::vector<double> forController(std::vector<std::vector<double>> toConvert);
std::vector<std::vector<double>> forLearner(std::vector<double> toConvert);

//used for communication between hyperneat learner and ext nn net controller
extern boost::shared_ptr<ExtNNConfig> cpg_network;
extern std::map<std::string, std::tuple<int,int,int>> neuron_coordinates;
extern CPPNEAT::GeneticEncodingPtr last;

std::string getHyper();
//converts a layered genotype to a layered phenotype
//only works if genotype->layered == true
boost::shared_ptr<LayeredExtNNConfig> convertForLayeredExtNN(CPPNEAT::GeneticEncodingPtr genotype); 
boost::shared_ptr<ExtNNConfig> convertForExtNNFromHyper(CPPNEAT::GeneticEncodingPtr genotype);
CPPNEAT::GeneticEncodingPtr convertForHyperFromExtNN(boost::shared_ptr<ExtNNConfig> config);
}
}



#endif /* REVOLVE_BRAIN_CONVERSION_H_ */
