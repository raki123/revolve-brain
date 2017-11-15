#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_

#include <map>
#include <string>
#include <vector>
#include <random>

#include "GeneticEncoding.h"

//class responsible for mutation
namespace CPPNEAT
{
  class Mutator
  {
    public:

    Mutator(
            std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec,
            double new_connection_sigma,
            int innovation_number,
            int max_attempts,
            std::vector< Neuron::Ntype > addable_neurons);

    static std::vector< Neuron::Ntype > AddableTypes(
            std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > _specifications);

    void make_starting_genotype_known(GeneticEncodingPtr _genotype);

    void LoadInnovationsFromFirst(const std::string &_path);

    void LoadInnovationsFromSecond(const std::string &_path);

    void RecordInnovations(const std::string &_path);

    void MutateNeuronParams(
            GeneticEncodingPtr genotype,
            double probability,
            double sigma);

    void MutateWeights(
            GeneticEncodingPtr genotype,
            double probability,
            double sigma);

    void MutateStructure(
            GeneticEncodingPtr genotype,
            double probability);

    bool MutateConnection(
            GeneticEncodingPtr genotype,
            double sigma);

    void MutateNeuron(
            GeneticEncodingPtr genotype,
            double sigma);

    void RemoveConnection(GeneticEncodingPtr genotype);

    void RemoveNeuron(GeneticEncodingPtr genotype);

    size_t AddNeuron(
            const NeuronPtr neuron,
            const GeneticEncodingPtr genotype,
            const ConnectionGenePtr split);

    size_t AddConnection(
            const size_t _from,
            const size_t _to,
            const double _weight,
            const GeneticEncodingPtr _genotype,
            const std::string &_socket);


    std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > BrainSpec()
    {
      return brain_spec;
    };

    void SetInnovationNumber(const size_t _innovationNumber)
    {
      this->innovationNumber_ = _innovationNumber;
    };

    private:
    std::map< std::pair< size_t, size_t >, size_t > connectionInnovations_;
    std::map< std::pair< size_t, size_t >, size_t > connectionSecondInnovations_;

    //<mark_from, mark_to> -> innovation_number
    //contains all connections that ever existed!
    /*<innovation_num_of_split_connection, added_neurons_type> ->
     * -> added_neurons_innovation_nums
     * since we want the ability to have multiple neurons of the same type
     * added between two neurons. It is necessary that we check if a new
     * neuron has already been added to the current genotype. If so we add
     * another neuron in order to store all the innovation numbers we need a
     * vector.
     */
    std::map< std::pair< size_t, Neuron::Ntype >, std::vector< size_t >>
            neuronInnovations_;
    std::map< std::pair< size_t, Neuron::Ntype >, std::vector< size_t >>
            neuronSecondInnovations_;

    std::map< size_t, size_t > secondToFirst;

    //contains only neurons that have been added by structural mutation
    std::map< Neuron::Ntype, Neuron::NeuronTypeSpec > brain_spec;
    double sigma_;
    size_t innovationNumber_;
    size_t max_attempts;
    std::vector< Neuron::Ntype > addable_neurons;

    std::mt19937 generator;
  };
}

#endif  //NEAT_MUTATOR_H_