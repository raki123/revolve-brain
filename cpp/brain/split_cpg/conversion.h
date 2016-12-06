#ifndef REVOLVE_BRAIN_CONVERSION_H_
#define REVOLVE_BRAIN_CONVERSION_H_

#include "../cppneat/genetic_encoding.h"
#include "../split_cpg/ext_nn.h"

namespace revolve {
namespace brain {

extern ExtNNController1::ExtNNConfig (*convertForController_)(CPPNEAT::GeneticEncodingPtr genotype);
extern CPPNEAT::GeneticEncodingPtr (*convertForLearner_)(ExtNNController1::ExtNNConfig config);

}
}

#endif /* REVOLVE_BRAIN_CONVERSION_H_ */
