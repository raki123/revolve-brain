#ifndef REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_
#define REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_

#include <iostream>

#include "ConvertingSplitBrain.h"
#include "Evaluator.h"

namespace revolve {
namespace brain {

template <typename Genome>
Genome
idem(Genome g)
{ return g; }

template <typename Genome>
class SimpleSplitBrain
        : public ConvSplitBrain<Genome, Genome>
{
public:
    SimpleSplitBrain(std::string modelName) :
            ConvSplitBrain<Genome, Genome>(&idem,
                                           &idem,
                                           modelName)
    {};

    virtual ~SimpleSplitBrain()
    {};
};

}
}

#endif // REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_
