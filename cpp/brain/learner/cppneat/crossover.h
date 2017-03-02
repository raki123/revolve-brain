#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_

#include "GeneticEncoding.h"

//crossover between genotypes
namespace CPPNEAT {
class Crossover
{
public:
    static GeneticEncodingPtr
    crossover(GeneticEncodingPtr genotype_more_fit,
              GeneticEncodingPtr genotype_less_fit);

};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_CROSSOVER_H_