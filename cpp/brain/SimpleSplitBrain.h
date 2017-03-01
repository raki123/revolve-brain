#ifndef REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_
#define REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_

#include <iostream>

#include "ConvertingSplitBrain.h"
#include "Evaluator.h"

namespace revolve {
namespace brain {

template <typename Genome> 
Genome idem(Genome g) { return g;}

template <typename Genome>
class SimpleSplitBrain : public ConvSplitBrain<Genome,Genome> {
public:
    SimpleSplitBrain(std::string modelName) : ConvSplitBrain<Genome,Genome>(&idem, &idem, modelName) { };
    virtual ~SimpleSplitBrain() {};
};

}
}

#endif /* REVOLVE_BRAIN_SIMPLE_SPLIT_BRAIN_H_ */
