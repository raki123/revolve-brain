#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_

#include "GeneticEncoding.h"

//crossover between genotypes
namespace CPPNEAT {
class Crossover
{
public:
    static GeneticEncodingPtr
    crossover(GeneticEncodingPtr _moreFitGenome,
              GeneticEncodingPtr _lessFitGenome);

};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_