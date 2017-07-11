#ifndef REVOLVEBRAIN_BRAIN_CONVERSION_H_
#define REVOLVEBRAIN_BRAIN_CONVERSION_H_

#include "brain/controller/RafCPGController.h"
#include "brain/controller/LayeredExtCPPN.h"
#include "brain/learner/NEATLearner.h"
#include "brain/learner/RLPowerLearner.h"
#include "brain/learner/cppneat/GeneticEncoding.h"

namespace revolve {
namespace brain {

//used for communication between cppneat learner and ext nn net controller
//the conversion methods work only when using standard neat
extern std::map<CPPNEAT::Neuron::Ntype, CPPNEAT::Neuron::NeuronTypeSpec> brain_spec;

extern std::map<int, size_t > input_map;

extern std::map<int, size_t > output_map;

void
set_brain_spec(bool hyperneat);

boost::shared_ptr<CPPNConfig>
convertForController(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertForLearner(boost::shared_ptr<CPPNConfig> config);

///////////////////////////////////////////////////////////////////////////////
/// HyperNEAT_CPG
///////////////////////////////////////////////////////////////////////////////
//used for communication between rlpower learner and ext nn weights controller
std::vector<double> convertPolicyToDouble(PolicyPtr genotype);

PolicyPtr convertDoubleToNull(std::vector<double> phenotype);
///////////////////////////////////////////////////////////////////////////////

//used for communication between hyperneat learner and ext nn net controller
extern boost::shared_ptr<CPPNConfig> cpg_network;

extern std::map<std::string, std::tuple<int, int, int>> neuron_coordinates;

extern CPPNEAT::GeneticEncodingPtr last_genotype_;

//converts a layered genotype to a layered phenotype
//only works if genotype->layered == true
boost::shared_ptr<LayeredExtNNConfig>
convertForLayeredExtNN(CPPNEAT::GeneticEncodingPtr genotype);

///////////////////////////////////////////////////////////////////////////////
/// RLPower_CPG~RLPower_CPPN
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CPPNConfig>
convertGeneticEncodingToCPPNConfig(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertCPPNConfigToGeneticEncoding(boost::shared_ptr<CPPNConfig> config);
///////////////////////////////////////////////////////////////////////////////

//used for communication between spline controller and hyperneat learner
//contains the coordinates of the actuators matching the order the actuators are given in the update method
//coordinate of actuators[0] is in sorted_coordinates[0]
extern std::vector<std::pair<int, int>> sorted_coordinates;

extern size_t spline_size;

extern size_t update_rate;

extern size_t cur_step;

//returns the starting network for hyperneat on splines
CPPNEAT::GeneticEncodingPtr get_hyper_neat_net_splines();

PolicyPtr
convertForSplinesFromHyper(CPPNEAT::GeneticEncodingPtr genotype);

CPPNEAT::GeneticEncodingPtr
convertForHyperFromSplines(PolicyPtr policy);
}
}


#endif // REVOLVEBRAIN_BRAIN_CONVERSION_H_
