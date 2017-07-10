#ifndef REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_
#define REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_

#include <iostream>

#include "ConverterSplitBrain.h"
#include "Evaluator.h"

namespace revolve {
namespace brain {

template <typename Genotype>
Genotype convertPolicyToPolicy(Genotype genotype)
{
    return genotype;
}

template <typename Genotype>
class SimpleSplitBrain
        : public ConverterSplitBrain<Genotype, Genotype>
{
public:

    SimpleSplitBrain(const std::string model_name)
            : ConverterSplitBrain<Genotype, Genotype>
                      (&convertPolicyToPolicy,
                       &convertPolicyToPolicy,
                       model_name)
    {};

    virtual ~SimpleSplitBrain()
    {};
};

}
}

#endif // REVOLVEBRAIN_BRAIN_SIMPLESPLITBRAIN_H_
