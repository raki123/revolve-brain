#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_MUTATOR_H_

#include <map>
#include <string>
#include <vector>
#include <random>

#include "GeneticEncoding.h"

//class responsible for mutation
namespace CPPNEAT {
class Mutator
{
public:

    Mutator(std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec,
            double new_connection_sigma,
            int innovation_number,
            int max_attempts,
            std::vector<Neuron::Ntype> addable_neurons,
            bool layered);

    static std::vector<Neuron::Ntype>
    get_addable_types(std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec);

    void make_starting_genotype_known(GeneticEncodingPtr genotype);

    void load_known_innovations(std::string yaml_path);

    void write_known_innovations(std::string yaml_path);

    void mutate_neuron_params(GeneticEncodingPtr genotype,
                              double probability,
                              double sigma);

    void mutate_weights(GeneticEncodingPtr genotype,
                        double probability,
                        double sigma);

    void mutate_structure(GeneticEncodingPtr genotype,
                          double probability);

    bool add_connection_mutation(GeneticEncodingPtr genotype,
                                 double sigma);

    void add_neuron_mutation(GeneticEncodingPtr genotype,
                             double sigma);

    void remove_connection_mutation(GeneticEncodingPtr genotype);

    void remove_neuron_mutation(GeneticEncodingPtr genotype);

    int add_neuron(NeuronPtr neuron,
                   GeneticEncodingPtr genotype,
                   ConnectionGenePtr split);

    int add_connection(int mark_from,
                       int mark_to,
                       double weight,
                       GeneticEncodingPtr genotype,
                       std::string socket);


    std::map<Neuron::Ntype, Neuron::NeuronTypeSpec>
    get_brain_spec()
    { return brain_spec; };

    void
    set_current_innovation_number(int innov_numb)
    { this->innovation_number = innov_numb; };

private:
    std::map<std::pair<int, int>, int> connection_innovations;            //<mark_from, mark_to> -> innovation_number
    //contains all connections that ever existed!
    std::map<std::pair<int, Neuron::Ntype>, std::vector<int>> neuron_innovations;        /*<innovation_number of split connection,
											 * neuron_type of added neuron> 
											 * -> 
											 * innovation_numbers of added neurons
											 * since we want the ability to have multiple neurons of the same type added between two neurons
											 * it is necessary that we check if a new neuron has already been added to the current genotype 
											 * if so we add another neuron 
											 * in order to store all the innovation numbers we need a vector*/
    //contains only neurons that have been added by structural mutation
    std::map<Neuron::Ntype, Neuron::NeuronTypeSpec> brain_spec;
    double new_connection_sigma;
    int innovation_number;
    int max_attempts;
    std::vector<Neuron::Ntype> addable_neurons;
//    bool is_layered_;
    std::mt19937 generator;
};
}

#endif  //NEAT_MUTATOR_H_