#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURONGENOME_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURONGENOME_H_

#include "Genome.h"
#include "CPPNNeuron.h"
#include "CPPNTypes.h"

//class combining meta and normal informations for neurons
namespace CPPNEAT {
class NeuronGene
        : public Gene
{

public:
    NeuronGene(NeuronPtr neuron,
               const size_t _innovationNumber = 0,
               const bool _enabled = true,
               const std::string _parentsName = "",
               const int _parentsIndex = -1)
            : Gene(_innovationNumber,
                   _enabled,
                   _parentsName,
                   _parentsIndex)
            , neuron(neuron)
    {
      this->type_ = Gene::NEURON_GENE;
    }

    NeuronGene(NeuronGene &_copy)
            : Gene(_copy.InnovationNumber(),
                   _copy.IsEnabled(),
                   _copy.ParentsName(),
                   _copy.ParentsIndex())
            , neuron(NeuronPtr(new Neuron(*(_copy.neuron))))
    {
        this->type_ = Gene::NEURON_GENE;
    }

public:
    NeuronPtr neuron;
};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURONGENOME_H_