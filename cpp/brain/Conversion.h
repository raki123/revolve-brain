#ifndef REVOLVEBRAIN_BRAIN_CONVERSION_H_
#define REVOLVEBRAIN_BRAIN_CONVERSION_H_

#include "brain/controller/ExtendedANN.h"
#include "brain/controller/LayeredExtendedANN.h"
#include "brain/learner/NEATLearner.h"
#include "brain/learner/RLPowerLearner.h"
#include "brain/learner/cppneat/GeneticEncoding.h"

namespace revolve {
namespace brain {

//used for communication between cppneat learner and ext nn net controller
//the conversion methods work only when using standard neat
extern std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

extern std::map<int, unsigned int> input_map;

extern std::map<int, unsigned int> output_map;

void
set_brain_spec(bool hyperneat);

boost::shared_ptr<ExtNNConfig>
convertForController(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertForLearner(boost::shared_ptr<ExtNNConfig> config);


//used for communication between rlpower learner and ext nn weights controller
std::vector<double>
forController(PolicyPtr toConvert);

PolicyPtr
forLearner(std::vector<double> toConvert);

//used for communication between hyperneat learner and ext nn net controller
extern boost::shared_ptr<ExtNNConfig> cpg_network;

extern std::map<std::string, std::tuple<int, int, int>> neuron_coordinates;

extern CPPNEAT::GeneticEncodingPtr last;

//converts a layered genotype to a layered phenotype
//only works if genotype->layered == true
boost::shared_ptr<LayeredExtNNConfig>
convertForLayeredExtNN(CPPNEAT::GeneticEncodingPtr genotype);

boost::shared_ptr<ExtNNConfig>
convertForExtNNFromHyper(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertForHyperFromExtNN(boost::shared_ptr<ExtNNConfig> config);

//used for communication between spline controller and hyperneat learner
//contains the coordinates of the actuators matching the order the actuators are given in the update method
//coordinate of actuators[0] is in sorted_coordinates[0]
extern std::vector<std::pair<int, int>> sorted_coordinates;

extern unsigned int spline_size;

extern unsigned int update_rate;

extern unsigned int cur_step;

//returns the starting network for hyperneat on splines
CPPNEAT::GeneticEncodingPtr
get_hyper_neat_net_splines();

PolicyPtr
convertForSplinesFromHyper(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertForHyperFromSplines(PolicyPtr policy);
}
}


#endif // REVOLVEBRAIN_BRAIN_CONVERSION_H_
