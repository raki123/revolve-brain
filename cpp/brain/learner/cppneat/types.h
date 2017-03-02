#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_TYPES_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_TYPES_H_

#include <boost/shared_ptr.hpp>

namespace CPPNEAT {
class Gene;

class Neuron;

class NeuronGene;

class ConnectionGene;

class GeneticEncoding;

class Mutator;

typedef boost::shared_ptr<Gene> GenePtr;

typedef boost::shared_ptr<Neuron> NeuronPtr;

typedef boost::shared_ptr<NeuronGene> NeuronGenePtr;

typedef boost::shared_ptr<ConnectionGene> ConnectionGenePtr;

typedef boost::shared_ptr<GeneticEncoding> GeneticEncodingPtr;

typedef boost::shared_ptr<Mutator> MutatorPtr;
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_TYPES_H_