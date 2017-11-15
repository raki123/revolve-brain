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
               int innov_number = 0,
               bool enabled = true,
               std::string parent_name = "",
               int parent_index = -1)
            :
            Gene(innov_number,
                 enabled,
                 parent_name,
                 parent_index)
            , neuron(neuron)
    { this->type_ = Gene::NEURON_GENE; }

    NeuronGene(NeuronGene &_copy)
            :
            Gene(_copy.InnovationNumber(),
                 _copy.isEnabled(),
                 _copy.get_parent_name(),
                 _copy.get_parent_index())
            , neuron(NeuronPtr(new Neuron(*(_copy.neuron))))
    { this->type_ = Gene::NEURON_GENE; }

public:
    NeuronPtr neuron;
};
}

#endif // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURONGENOME_H_