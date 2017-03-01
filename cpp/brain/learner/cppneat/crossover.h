#ifndef NEAT_GENETIC_ENCODING_CROSSOVER_H_
#define NEAT_GENETIC_ENCODING_CROSSOVER_H_

#include "GeneticEncoding.h"

//crossover between genotypes
namespace CPPNEAT
{
class Crossover
{
public:
    static GeneticEncodingPtr
    crossover(GeneticEncodingPtr genotype_more_fit,
              GeneticEncodingPtr genotype_less_fit);

};
}

#endif //NEAT_CROSSOVER_H_